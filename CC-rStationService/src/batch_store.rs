//! Batch Store - CRUD Operations for Batch Tasks
//!
//! This module provides persistent storage and CRUD operations for batch tasks.

use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::{Arc, RwLock};

use anyhow::{Context, Result};
use chrono::{DateTime, Utc};
use rusqlite::{params, Connection};
use uuid::Uuid;

use crate::batch::{
    BatchExecutionItem, BatchExecutionStatus, BatchParameterValue, BatchTask, BatchTaskFilter,
    BatchTaskSortField, BatchTaskStats, BatchTaskStatus, ExecutionPolicy, TargetSelector,
};
use crate::scripts::CommandScript;

/// In-memory cache for batch tasks
pub struct BatchTaskCache {
    tasks: RwLock<HashMap<Uuid, BatchTask>>,
    stats: RwLock<BatchTaskStats>,
}

impl BatchTaskCache {
    pub fn new() -> Self {
        Self {
            tasks: RwLock::new(HashMap::new()),
            stats: RwLock::new(BatchTaskStats::default()),
        }
    }

    pub fn get(&self, id: &Uuid) -> Option<BatchTask> {
        self.tasks.read().ok()?.get(id).cloned()
    }

    pub fn get_all(&self) -> Vec<BatchTask> {
        self.tasks
            .read()
            .map(|tasks| tasks.values().cloned().collect())
            .unwrap_or_default()
    }

    pub fn insert(&self, task: BatchTask) {
        let mut tasks = self.tasks.write().unwrap();
        tasks.insert(task.id, task.clone());
        drop(tasks);
        self.update_stats();
    }

    pub fn update(&self, task: &BatchTask) -> Option<BatchTask> {
        let mut tasks = self.tasks.write().unwrap();
        if tasks.contains_key(&task.id) {
            let old = tasks.get(&task.id).cloned();
            tasks.insert(task.id, task.clone());
            drop(tasks);
            self.update_stats();
            old
        } else {
            None
        }
    }

    pub fn remove(&self, id: &Uuid) -> Option<BatchTask> {
        let mut tasks = self.tasks.write().unwrap();
        let removed = tasks.remove(id);
        drop(tasks);
        if removed.is_some() {
            self.update_stats();
        }
        removed
    }

    pub fn filter(&self, filter: &BatchTaskFilter) -> Vec<BatchTask> {
        let tasks: Vec<BatchTask> = self.get_all();

        let mut filtered: Vec<BatchTask> = tasks
            .into_iter()
            .filter(|task| {
                if let Some(status) = &filter.status {
                    if task.status != *status {
                        return false;
                    }
                }
                if let Some(created_by) = &filter.created_by {
                    if task.created_by.as_deref() != Some(created_by.as_str()) {
                        return false;
                    }
                }
                true
            })
            .collect();

        // Sort
        let sort_by = filter.sort_by;
        let sort_desc = filter.sort_desc;
        filtered.sort_by(|a, b| {
            let cmp = match sort_by {
                BatchTaskSortField::CreatedAt => a.created_at.cmp(&b.created_at),
                BatchTaskSortField::UpdatedAt => a.updated_at.cmp(&b.updated_at),
                BatchTaskSortField::Name => a.name.cmp(&b.name),
                BatchTaskSortField::Status => a.status.cmp(&b.status),
                BatchTaskSortField::Progress => a.progress.cmp(&b.progress),
            };
            if sort_desc {
                cmp.reverse()
            } else {
                cmp
            }
        });

        // Pagination (limit 0 means no limit)
        let offset = filter.offset as usize;
        let limit = if filter.limit == 0 { usize::MAX } else { filter.limit as usize };
        filtered.into_iter().skip(offset).take(limit).collect()
    }

    fn update_stats(&self) {
        let tasks = self.tasks.read().unwrap();
        let mut stats = self.stats.write().unwrap();

        stats.total_tasks = tasks.len() as u64;
        stats.pending_tasks = tasks
            .values()
            .filter(|t| t.status == BatchTaskStatus::Pending)
            .count() as u64;
        stats.running_tasks = tasks
            .values()
            .filter(|t| t.status == BatchTaskStatus::Running)
            .count() as u64;
        stats.completed_tasks = tasks
            .values()
            .filter(|t| t.status == BatchTaskStatus::Completed)
            .count() as u64;
        stats.failed_tasks = tasks
            .values()
            .filter(|t| {
                t.status == BatchTaskStatus::Failed
                    || t.status == BatchTaskStatus::PartialFailure
            })
            .count() as u64;

        let mut total = 0u64;
        let mut successful = 0u64;
        let mut failed = 0u64;
        for task in tasks.values() {
            for item in &task.execution_items {
                total += 1;
                match item.status {
                    BatchExecutionStatus::Success => successful += 1,
                    BatchExecutionStatus::Failed => failed += 1,
                    _ => {}
                }
            }
        }
        stats.total_executions = total;
        stats.successful_executions = successful;
        stats.failed_executions = failed;
    }

    pub fn stats(&self) -> BatchTaskStats {
        self.stats.read().unwrap().clone()
    }
}

impl Default for BatchTaskCache {
    fn default() -> Self {
        Self::new()
    }
}

/// Batch task store with SQLite persistence.
pub struct BatchTaskStore {
    cache: Arc<BatchTaskCache>,
    db_path: PathBuf,
}

impl BatchTaskStore {
    /// Create a new batch task store with the given database path.
    pub fn new(db_path: PathBuf) -> Result<Self> {
        let store = Self {
            cache: Arc::new(BatchTaskCache::new()),
            db_path,
        };
        store.init_db()?;
        store.load_from_db()?;
        Ok(store)
    }

    /// Initialize the database schema.
    fn init_db(&self) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open batch store DB at {}", self.db_path.display()))?;

        conn.execute(
            "CREATE TABLE IF NOT EXISTS batch_tasks (
                id TEXT PRIMARY KEY,
                name TEXT NOT NULL,
                description TEXT,
                target_selector TEXT NOT NULL,
                execution_policy TEXT NOT NULL,
                script_id TEXT NOT NULL,
                script_content TEXT NOT NULL,
                default_parameters TEXT NOT NULL,
                status TEXT NOT NULL,
                progress INTEGER NOT NULL DEFAULT 0,
                total_items INTEGER NOT NULL DEFAULT 0,
                created_at TEXT NOT NULL,
                updated_at TEXT NOT NULL,
                started_at TEXT,
                completed_at TEXT,
                created_by TEXT
            )",
            [],
        )?;

        conn.execute(
            "CREATE TABLE IF NOT EXISTS batch_execution_items (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                task_id TEXT NOT NULL,
                station_id TEXT NOT NULL,
                script_id TEXT NOT NULL,
                parameters TEXT NOT NULL,
                result TEXT,
                status TEXT NOT NULL,
                FOREIGN KEY (task_id) REFERENCES batch_tasks(id) ON DELETE CASCADE
            )",
            [],
        )?;

        conn.execute(
            "CREATE INDEX IF NOT EXISTS idx_execution_items_task_id ON batch_execution_items(task_id)",
            [],
        )?;

        Ok(())
    }

    /// Load tasks from the database into cache.
    fn load_from_db(&self) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open batch store DB at {}", self.db_path.display()))?;

        let mut stmt = conn.prepare(
            "SELECT id, name, description, target_selector, execution_policy,
                    script_id, script_content, default_parameters, status,
                    progress, total_items, created_at, updated_at, started_at,
                    completed_at, created_by
             FROM batch_tasks ORDER BY created_at DESC",
        )?;

        let row_data_list: Vec<RowData> = stmt
            .query_map([], |row| {
                let target_selector_json: String = row.get(3)?;
                let execution_policy_json: String = row.get(4)?;
                let default_params_json: String = row.get(7)?;
                let created_at_str: String = row.get(11)?;
                let updated_at_str: String = row.get(12)?;
                let started_at_str: Option<String> = row.get(13)?;
                let completed_at_str: Option<String> = row.get(14)?;
                let status_str: String = row.get(8)?;

                Ok(RowData {
                    id: row.get(0)?,
                    name: row.get(1)?,
                    description: row.get(2)?,
                    target_selector: target_selector_json,
                    execution_policy: execution_policy_json,
                    script_id: row.get(5)?,
                    script_content: row.get(6)?,
                    default_parameters: default_params_json,
                    status: status_str,
                    progress: row.get(9)?,
                    total_items: row.get(10)?,
                    created_at: created_at_str,
                    updated_at: updated_at_str,
                    started_at: started_at_str,
                    completed_at: completed_at_str,
                    created_by: row.get(15)?,
                })
            })?
            .filter_map(|r| r.ok())
            .collect();

        for task_data in row_data_list {
            if let Ok(task) = self.row_data_to_task(task_data, &conn) {
                self.cache.insert(task);
            }
        }

        Ok(())
    }

    fn row_data_to_task(&self, data: RowData, conn: &Connection) -> Result<BatchTask> {
        let target_selector: TargetSelector =
            serde_json::from_str(&data.target_selector).unwrap_or_default();
        let execution_policy: ExecutionPolicy =
            serde_json::from_str(&data.execution_policy).unwrap_or_default();
        let default_parameters: Vec<BatchParameterValue> =
            serde_json::from_str(&data.default_parameters).unwrap_or_default();
        let status: BatchTaskStatus =
            serde_json::from_str(&data.status).unwrap_or_default();

        let created_at = DateTime::parse_from_rfc3339(&data.created_at)
            .map(|dt| dt.with_timezone(&Utc))
            .unwrap_or_else(|_| Utc::now());
        let updated_at = DateTime::parse_from_rfc3339(&data.updated_at)
            .map(|dt| dt.with_timezone(&Utc))
            .unwrap_or_else(|_| Utc::now());
        let started_at = data
            .started_at
            .and_then(|s| DateTime::parse_from_rfc3339(&s).ok())
            .map(|dt| dt.with_timezone(&Utc));
        let completed_at = data
            .completed_at
            .and_then(|s| DateTime::parse_from_rfc3339(&s).ok())
            .map(|dt| dt.with_timezone(&Utc));

        let execution_items = self.load_execution_items(&data.id, conn)?;

        Ok(BatchTask {
            id: Uuid::parse_str(&data.id).unwrap_or_else(|_| Uuid::new_v4()),
            name: data.name,
            description: data.description,
            target_selector,
            execution_policy,
            script_id: Uuid::parse_str(&data.script_id).unwrap_or_else(|_| Uuid::new_v4()),
            script_content: data.script_content,
            default_parameters,
            execution_items,
            status,
            progress: data.progress,
            total_items: data.total_items,
            created_at,
            updated_at,
            started_at,
            completed_at,
            created_by: data.created_by,
        })
    }

    fn load_execution_items(&self, task_id: &str, conn: &Connection) -> Result<Vec<BatchExecutionItem>> {
        let mut stmt = conn.prepare(
            "SELECT station_id, script_id, parameters, result, status FROM batch_execution_items WHERE task_id = ?",
        )?;

        let items: Vec<BatchExecutionItem> = stmt
            .query_map([task_id], |row| {
                let params_json: String = row.get(2)?;
                let result_json: Option<String> = row.get(3)?;
                let status_str: String = row.get(4)?;

                Ok(ExecutionItemData {
                    station_id: row.get(0)?,
                    script_id: row.get(1)?,
                    parameters: params_json,
                    result: result_json,
                    status: status_str,
                })
            })?
            .filter_map(|r| r.ok())
            .map(|data| {
                let parameters: Vec<BatchParameterValue> =
                    serde_json::from_str(&data.parameters).unwrap_or_default();
                let result: Option<crate::scripts::ScriptExecutionResult> = data
                    .result
                    .and_then(|r| serde_json::from_str(&r).ok());
                let status: BatchExecutionStatus =
                    serde_json::from_str(&data.status).unwrap_or_default();

                BatchExecutionItem {
                    station_id: data.station_id,
                    script_id: Uuid::parse_str(&data.script_id).unwrap_or_else(|_| Uuid::new_v4()),
                    parameters,
                    result,
                    status,
                }
            })
            .collect();

        Ok(items)
    }

    // CRUD Operations

    /// Create a new batch task.
    pub fn create(&self, task: &BatchTask) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open batch store DB at {}", self.db_path.display()))?;

        let target_selector_json = serde_json::to_string(&task.target_selector)?;
        let execution_policy_json = serde_json::to_string(&task.execution_policy)?;
        let default_params_json = serde_json::to_string(&task.default_parameters)?;
        let status_json = serde_json::to_string(&task.status)?;

        conn.execute(
            "INSERT INTO batch_tasks (id, name, description, target_selector, execution_policy,
                                     script_id, script_content, default_parameters, status,
                                     progress, total_items, created_at, updated_at, started_at,
                                     completed_at, created_by)
             VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16)",
            params![
                task.id.to_string(),
                task.name,
                task.description,
                target_selector_json,
                execution_policy_json,
                task.script_id.to_string(),
                task.script_content,
                default_params_json,
                status_json,
                task.progress,
                task.total_items,
                task.created_at.to_rfc3339(),
                task.updated_at.to_rfc3339(),
                task.started_at.map(|dt| dt.to_rfc3339()),
                task.completed_at.map(|dt| dt.to_rfc3339()),
                task.created_by,
            ],
        )?;

        // Insert execution items
        for item in &task.execution_items {
            let params_json = serde_json::to_string(&item.parameters)?;
            let result_json = item.result.as_ref().map(|r| serde_json::to_string(r).ok()).flatten();
            let status_json = serde_json::to_string(&item.status)?;

            conn.execute(
                "INSERT INTO batch_execution_items (task_id, station_id, script_id, parameters, result, status)
                 VALUES (?1, ?2, ?3, ?4, ?5, ?6)",
                params![
                    task.id.to_string(),
                    item.station_id,
                    item.script_id.to_string(),
                    params_json,
                    result_json,
                    status_json,
                ],
            )?;
        }

        self.cache.insert(task.clone());
        Ok(())
    }

    /// Get a batch task by ID.
    pub fn get(&self, id: &Uuid) -> Option<BatchTask> {
        self.cache.get(id)
    }

    /// Get all batch tasks.
    pub fn get_all(&self) -> Vec<BatchTask> {
        self.cache.get_all()
    }

    /// Update a batch task.
    pub fn update(&self, task: &BatchTask) -> Result<Option<BatchTask>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open batch store DB at {}", self.db_path.display()))?;

        let target_selector_json = serde_json::to_string(&task.target_selector)?;
        let execution_policy_json = serde_json::to_string(&task.execution_policy)?;
        let default_params_json = serde_json::to_string(&task.default_parameters)?;
        let status_json = serde_json::to_string(&task.status)?;

        let affected = conn.execute(
            "UPDATE batch_tasks SET name = ?2, description = ?3, target_selector = ?4,
                                    execution_policy = ?5, script_id = ?6, script_content = ?7,
                                    default_parameters = ?8, status = ?9, progress = ?10,
                                    total_items = ?11, updated_at = ?12, started_at = ?13,
                                    completed_at = ?14, created_by = ?15
             WHERE id = ?1",
            params![
                task.id.to_string(),
                task.name,
                task.description,
                target_selector_json,
                execution_policy_json,
                task.script_id.to_string(),
                task.script_content,
                default_params_json,
                status_json,
                task.progress,
                task.total_items,
                task.updated_at.to_rfc3339(),
                task.started_at.map(|dt| dt.to_rfc3339()),
                task.completed_at.map(|dt| dt.to_rfc3339()),
                task.created_by,
            ],
        )?;

        if affected > 0 {
            // Update execution items - delete and re-insert
            conn.execute(
                "DELETE FROM batch_execution_items WHERE task_id = ?1",
                [task.id.to_string()],
            )?;

            for item in &task.execution_items {
                let params_json = serde_json::to_string(&item.parameters)?;
                let result_json = item.result.as_ref().map(|r| serde_json::to_string(r).ok()).flatten();
                let status_json = serde_json::to_string(&item.status)?;

                conn.execute(
                    "INSERT INTO batch_execution_items (task_id, station_id, script_id, parameters, result, status)
                     VALUES (?1, ?2, ?3, ?4, ?5, ?6)",
                    params![
                        task.id.to_string(),
                        item.station_id,
                        item.script_id.to_string(),
                        params_json,
                        result_json,
                        status_json,
                    ],
                )?;
            }

            self.cache.update(task);
            Ok(self.cache.get(&task.id))
        } else {
            Ok(None)
        }
    }

    /// Delete a batch task by ID.
    pub fn delete(&self, id: &Uuid) -> Result<Option<BatchTask>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open batch store DB at {}", self.db_path.display()))?;

        let affected = conn.execute(
            "DELETE FROM batch_tasks WHERE id = ?1",
            [id.to_string()],
        )?;

        if affected > 0 {
            Ok(self.cache.remove(id))
        } else {
            Ok(None)
        }
    }

    /// Filter batch tasks by criteria.
    pub fn filter(&self, filter: &BatchTaskFilter) -> Vec<BatchTask> {
        self.cache.filter(filter)
    }

    /// Get batch task statistics.
    pub fn stats(&self) -> BatchTaskStats {
        self.cache.stats()
    }
}

#[derive(Debug)]
struct RowData {
    id: String,
    name: String,
    description: Option<String>,
    target_selector: String,
    execution_policy: String,
    script_id: String,
    script_content: String,
    default_parameters: String,
    status: String,
    progress: u32,
    total_items: u32,
    created_at: String,
    updated_at: String,
    started_at: Option<String>,
    completed_at: Option<String>,
    created_by: Option<String>,
}

#[derive(Debug)]
struct ExecutionItemData {
    station_id: String,
    script_id: String,
    parameters: String,
    result: Option<String>,
    status: String,
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::env::temp_dir;
    use crate::batch::BatchExecutionStatus;

    #[test]
    fn test_batch_task_crud() {
        let db_path = temp_dir().join("batch_test.db");
        // Clean up any stale data from previous test runs
        std::fs::remove_file(db_path.clone()).ok();
        let store = BatchTaskStore::new(db_path.clone()).unwrap();

        let script = CommandScript::new("test script", "echo hello");
        let mut task = BatchTask::new("Test Task", &script);
        task = task.with_description("A test batch task");
        task = task.with_execution_policy(ExecutionPolicy::Sequential);
        task = task.with_default_parameter("param1", "value1");

        // Create
        store.create(&task).unwrap();
        assert!(store.get(&task.id).is_some());

        // Read
        let retrieved = store.get(&task.id).unwrap();
        assert_eq!(retrieved.name, "Test Task");
        assert_eq!(retrieved.description, Some("A test batch task".to_string()));

        // Update
        let mut updated = retrieved;
        updated.status = BatchTaskStatus::Running;
        updated.started_at = Some(Utc::now());
        store.update(&updated).unwrap();

        let retrieved2 = store.get(&task.id).unwrap();
        assert_eq!(retrieved2.status, BatchTaskStatus::Running);

        // Delete
        store.delete(&task.id).unwrap();
        assert!(store.get(&task.id).is_none());

        // Clean up
        std::fs::remove_file(db_path).ok();
    }

    #[test]
    fn test_batch_task_filter() {
        let db_path = temp_dir().join("batch_filter_test.db");
        // Clean up any stale data from previous test runs
        std::fs::remove_file(db_path.clone()).ok();
        let store = BatchTaskStore::new(db_path.clone()).unwrap();

        let script = CommandScript::new("test script", "echo hello");

        // Create tasks with different statuses
        for i in 0..5 {
            let mut task = BatchTask::new(format!("Task {}", i), &script);
            task.status = match i {
                0..=1 => BatchTaskStatus::Completed,
                2..=3 => BatchTaskStatus::Running,
                _ => BatchTaskStatus::Pending,
            };
            store.create(&task).unwrap();
        }

        // Verify tasks are in cache
        let all_tasks = store.get_all();
        assert_eq!(all_tasks.len(), 5, "Should have 5 tasks in cache");

        // Filter by status
        let filter = BatchTaskFilter {
            status: Some(BatchTaskStatus::Completed),
            ..Default::default()
        };
        let filtered = store.filter(&filter);
        assert_eq!(filtered.len(), 2, "Should have 2 completed tasks");

        // Filter by pending
        let filter = BatchTaskFilter {
            status: Some(BatchTaskStatus::Pending),
            ..Default::default()
        };
        let filtered = store.filter(&filter);
        assert_eq!(filtered.len(), 1, "Should have 1 pending task");

        // Clean up
        std::fs::remove_file(db_path).ok();
    }

    #[test]
    fn test_batch_task_stats() {
        let db_path = temp_dir().join("batch_stats_test.db");
        let store = BatchTaskStore::new(db_path.clone()).unwrap();

        let script = CommandScript::new("test script", "echo hello");

        // Create tasks with execution items
        for i in 0..3 {
            let mut task = BatchTask::new(format!("Task {}", i), &script);
            task.execution_items.push(BatchExecutionItem {
                station_id: format!("station-{}", i),
                script_id: script.id,
                parameters: Vec::new(),
                result: None,
                status: BatchExecutionStatus::Success,
            });
            task.execution_items.push(BatchExecutionItem {
                station_id: format!("station-{}-2", i),
                script_id: script.id,
                parameters: Vec::new(),
                result: None,
                status: BatchExecutionStatus::Failed,
            });
            task.status = BatchTaskStatus::Completed;
            store.create(&task).unwrap();
        }

        let stats = store.stats();
        assert_eq!(stats.total_tasks, 3);
        assert_eq!(stats.total_executions, 6);
        assert_eq!(stats.successful_executions, 3);
        assert_eq!(stats.failed_executions, 3);

        // Clean up
        std::fs::remove_file(db_path).ok();
    }
}
