//! Script Store - SQLite Persistence Layer
//!
//! This module provides persistent storage for command scripts using SQLite.

use crate::scripts::{
    CommandScript, ParameterType, ScriptExecutionResult, ScriptExport, ScriptFilter,
    ScriptParameter, ScriptSortField, ScriptStats, ScriptType,
};
use chrono::{DateTime, TimeZone, Utc};
use rusqlite::{params, Connection, Result as SqliteResult};
use std::path::Path;
use std::sync::{Arc, Mutex};
use uuid::Uuid;

/// Errors that can occur during script storage operations.
#[derive(Debug, thiserror::Error)]
pub enum ScriptStoreError {
    /// Database error
    #[error("database error: {0}")]
    Database(#[from] rusqlite::Error),

    /// Script not found
    #[error("script not found: {0}")]
    NotFound(Uuid),

    /// Serialization error
    #[error("serialization error: {0}")]
    Serialization(#[from] serde_json::Error),

    /// Validation error
    #[error("validation error: {0}")]
    Validation(String),

    /// IO error
    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),
}

pub type ScriptStoreResult<T> = Result<T, ScriptStoreError>;

/// SQLite-based script storage.
#[derive(Debug, Clone)]
pub struct ScriptStore {
    conn: Arc<Mutex<Connection>>,
}

impl ScriptStore {
    /// Open or create a script database at the given path
    pub fn open<P: AsRef<Path>>(path: P) -> ScriptStoreResult<Self> {
        let conn = Connection::open(path)?;
        let store = Self {
            conn: Arc::new(Mutex::new(conn)),
        };
        store.init_schema()?;
        Ok(store)
    }

    /// Open an in-memory database (for testing)
    pub fn in_memory() -> ScriptStoreResult<Self> {
        let conn = Connection::open_in_memory()?;
        let store = Self {
            conn: Arc::new(Mutex::new(conn)),
        };
        store.init_schema()?;
        Ok(store)
    }

    /// Initialize the database schema
    fn init_schema(&self) -> ScriptStoreResult<()> {
        let conn = self.conn.lock().unwrap();

        conn.execute_batch(
            r#"
            CREATE TABLE IF NOT EXISTS scripts (
                id TEXT PRIMARY KEY,
                name TEXT NOT NULL,
                description TEXT,
                script_type TEXT NOT NULL DEFAULT 'shell',
                content TEXT NOT NULL,
                parameters TEXT NOT NULL DEFAULT '[]',
                is_template INTEGER NOT NULL DEFAULT 0,
                is_favorite INTEGER NOT NULL DEFAULT 0,
                tags TEXT NOT NULL DEFAULT '[]',
                created_by TEXT,
                created_at TEXT NOT NULL,
                updated_at TEXT NOT NULL,
                version INTEGER NOT NULL DEFAULT 1,
                usage_count INTEGER NOT NULL DEFAULT 0,
                selected_options TEXT NOT NULL DEFAULT '{}'
            );

            CREATE INDEX IF NOT EXISTS idx_scripts_name ON scripts(name);
            CREATE INDEX IF NOT EXISTS idx_scripts_script_type ON scripts(script_type);
            CREATE INDEX IF NOT EXISTS idx_scripts_is_favorite ON scripts(is_favorite);
            CREATE INDEX IF NOT EXISTS idx_scripts_created_at ON scripts(created_at);
            CREATE INDEX IF NOT EXISTS idx_scripts_updated_at ON scripts(updated_at);

            CREATE TABLE IF NOT EXISTS script_executions (
                id TEXT PRIMARY KEY,
                script_id TEXT NOT NULL,
                exit_code INTEGER,
                stdout TEXT NOT NULL DEFAULT '',
                stderr TEXT NOT NULL DEFAULT '',
                duration_ms INTEGER NOT NULL DEFAULT 0,
                success INTEGER NOT NULL DEFAULT 0,
                executed_at TEXT NOT NULL,
                error TEXT,
                FOREIGN KEY (script_id) REFERENCES scripts(id) ON DELETE CASCADE
            );

            CREATE INDEX IF NOT EXISTS idx_executions_script_id ON script_executions(script_id);
            CREATE INDEX IF NOT EXISTS idx_executions_executed_at ON script_executions(executed_at);
            "#,
        )?;

        Ok(())
    }

    /// Create a new script
    pub fn create(&self, script: &CommandScript) -> ScriptStoreResult<CommandScript> {
        let conn = self.conn.lock().unwrap();

        let parameters_json = serde_json::to_string(&script.parameters)?;
        let tags_json = serde_json::to_string(&script.tags)?;
        let selected_options_json = serde_json::to_string(&script.selected_options)?;

        let rows_affected = conn.execute(
            r#"
            INSERT INTO scripts (
                id, name, description, script_type, content, parameters,
                is_template, is_favorite, tags, created_by, created_at,
                updated_at, version, usage_count, selected_options
            ) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15)
            "#,
            params![
                script.id.to_string(),
                script.name,
                script.description,
                script.script_type.to_string(),
                script.content,
                parameters_json,
                script.is_template as i32,
                script.is_favorite as i32,
                tags_json,
                script.created_by,
                script.created_at.to_rfc3339(),
                script.updated_at.to_rfc3339(),
                script.version,
                script.usage_count,
                selected_options_json,
            ],
        )?;
        eprintln!("DEBUG create(): inserted {} rows for script '{}'", rows_affected, script.name);

        Ok(script.clone())
    }

    /// Get a script by ID
    pub fn get(&self, id: Uuid) -> ScriptStoreResult<CommandScript> {
        let conn = self.conn.lock().unwrap();

        let mut stmt = conn.prepare(
            r#"
            SELECT id, name, description, script_type, content, parameters,
                   is_template, is_favorite, tags, created_by, created_at,
                   updated_at, version, usage_count, selected_options
            FROM scripts WHERE id = ?1
            "#,
        )?;

        let script = stmt.query_row(params![id.to_string()], |row| {
            Ok(row_to_script(row)?)
        })?;

        Ok(script)
    }

    /// Update an existing script
    pub fn update(&self, script: &CommandScript) -> ScriptStoreResult<CommandScript> {
        let conn = self.conn.lock().unwrap();

        let parameters_json = serde_json::to_string(&script.parameters)?;
        let tags_json = serde_json::to_string(&script.tags)?;
        let selected_options_json = serde_json::to_string(&script.selected_options)?;

        // Increment version on update
        let new_version = script.version + 1;

        let rows_affected = conn.execute(
            r#"
            UPDATE scripts SET
                name = ?2,
                description = ?3,
                script_type = ?4,
                content = ?5,
                parameters = ?6,
                is_template = ?7,
                is_favorite = ?8,
                tags = ?9,
                updated_at = ?10,
                version = ?11,
                usage_count = ?12,
                selected_options = ?13
            WHERE id = ?1
            "#,
            params![
                script.id.to_string(),
                script.name,
                script.description,
                script.script_type.to_string(),
                script.content,
                parameters_json,
                script.is_template as i32,
                script.is_favorite as i32,
                tags_json,
                script.updated_at.to_rfc3339(),
                new_version,
                script.usage_count,
                selected_options_json,
            ],
        )?;

        if rows_affected == 0 {
            return Err(ScriptStoreError::NotFound(script.id));
        }

        // Return updated script with new version
        let mut updated = script.clone();
        updated.version = new_version;
        Ok(updated)
    }

    /// Delete a script by ID
    pub fn delete(&self, id: Uuid) -> ScriptStoreResult<()> {
        let conn = self.conn.lock().unwrap();

        let rows_affected = conn.execute("DELETE FROM scripts WHERE id = ?1", params![id.to_string()])?;

        if rows_affected == 0 {
            return Err(ScriptStoreError::NotFound(id));
        }

        Ok(())
    }

    /// List scripts with optional filtering
    pub fn list(&self, filter: &ScriptFilter) -> ScriptStoreResult<Vec<CommandScript>> {
        let conn = self.conn.lock().unwrap();

        let sql_base = "SELECT id, name, description, script_type, content, parameters,
                   is_template, is_favorite, tags, created_by, created_at,
                   updated_at, version, usage_count, selected_options
            FROM scripts WHERE 1=1";

        let mut conditions = Vec::new();

        if let Some(ref name_contains) = filter.name_contains {
            conditions.push(format!("name LIKE '%%{}%'", name_contains));
        }

        if let Some(ref script_type) = filter.script_type {
            conditions.push(format!("script_type = '{}'", script_type));
        }

        if let Some(ref tags) = filter.tags {
            for tag in tags {
                conditions.push(format!("tags LIKE '%{}%'", tag));
            }
        }

        if filter.favorites_only.unwrap_or(false) {
            conditions.push("is_favorite = 1".to_string());
        }

        if filter.templates_only.unwrap_or(false) {
            conditions.push("is_template = 1".to_string());
        }

        if let Some(ref created_by) = filter.created_by {
            conditions.push(format!("created_by = '{}'", created_by));
        }

        let mut sql = sql_base.to_string();
        for cond in &conditions {
            sql.push_str(" AND ");
            sql.push_str(cond);
        }

        let sort_field = match filter.sort_by {
            ScriptSortField::Name => "name",
            ScriptSortField::CreatedAt => "created_at",
            ScriptSortField::UpdatedAt => "updated_at",
            ScriptSortField::UsageCount => "usage_count",
            ScriptSortField::Version => "version",
        };
        let sort_dir = if filter.sort_desc { "DESC" } else { "ASC" };
        sql.push_str(&format!(" ORDER BY {} {}", sort_field, sort_dir));

        let mut stmt = conn.prepare(&sql)?;
        
        let scripts: Vec<CommandScript> = stmt
            .query_map([], |row| Ok(row_to_script(row)?))?
            .filter_map(|r| r.ok())
            .collect();

        eprintln!("DEBUG list(): returning {} scripts", scripts.len());
        Ok(scripts)
    }

    /// Toggle favorite status for a script
    pub fn toggle_favorite(&self, id: Uuid) -> ScriptStoreResult<CommandScript> {
        let conn = self.conn.lock().unwrap();

        conn.execute(
            r#"
            UPDATE scripts SET
                is_favorite = CASE WHEN is_favorite THEN 0 ELSE 1 END,
                updated_at = ?2
            WHERE id = ?1
            "#,
            params![id.to_string(), Utc::now().to_rfc3339()],
        )?;

        drop(conn);
        self.get(id)
    }

    /// Increment usage count for a script
    pub fn increment_usage(&self, id: Uuid) -> ScriptStoreResult<()> {
        let conn = self.conn.lock().unwrap();

        conn.execute(
            r#"
            UPDATE scripts SET
                usage_count = usage_count + 1,
                updated_at = ?2
            WHERE id = ?1
            "#,
            params![id.to_string(), Utc::now().to_rfc3339()],
        )?;

        Ok(())
    }

    /// Get script statistics
    pub fn stats(&self) -> ScriptStoreResult<ScriptStats> {
        let conn = self.conn.lock().unwrap();

        let total_scripts: u64 = conn.query_row(
            "SELECT COUNT(*) FROM scripts",
            [],
            |row| row.get(0),
        )?;

        let template_count: u64 = conn.query_row(
            "SELECT COUNT(*) FROM scripts WHERE is_template = 1",
            [],
            |row| row.get(0),
        )?;

        let favorite_count: u64 = conn.query_row(
            "SELECT COUNT(*) FROM scripts WHERE is_favorite = 1",
            [],
            |row| row.get(0),
        )?;

        let most_used_id: Option<String> = conn.query_row(
            "SELECT id FROM scripts ORDER BY usage_count DESC LIMIT 1",
            [],
            |row| row.get(0),
        ).ok();

        let total_executions: u64 = conn.query_row(
            "SELECT COUNT(*) FROM script_executions",
            [],
            |row| row.get(0),
        )?;

        Ok(ScriptStats {
            total_scripts,
            template_count,
            favorite_count,
            most_used: most_used_id.and_then(|id| Uuid::parse_str(&id).ok()),
            total_executions,
        })
    }

    /// Save an execution result
    pub fn save_execution(&self, result: &ScriptExecutionResult) -> ScriptStoreResult<()> {
        let conn = self.conn.lock().unwrap();

        conn.execute(
            r#"
            INSERT INTO script_executions (
                id, script_id, exit_code, stdout, stderr,
                duration_ms, success, executed_at, error
            ) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9)
            "#,
            params![
                result.execution_id.to_string(),
                result.script_id.to_string(),
                result.exit_code,
                result.stdout,
                result.stderr,
                result.duration_ms as i64,
                result.success as i32,
                result.executed_at.to_rfc3339(),
                result.error,
            ],
        )?;

        Ok(())
    }

    /// Get execution history for a script
    pub fn get_executions(
        &self,
        script_id: Uuid,
        limit: u64,
    ) -> ScriptStoreResult<Vec<ScriptExecutionResult>> {
        let conn = self.conn.lock().unwrap();

        let mut stmt = conn.prepare(
            r#"
            SELECT id, script_id, exit_code, stdout, stderr,
                   duration_ms, success, executed_at, error
            FROM script_executions
            WHERE script_id = ?1
            ORDER BY executed_at DESC
            LIMIT ?2
            "#,
        )?;

        let results = stmt
            .query_map(params![script_id.to_string(), limit as i64], |row| {
                Ok(row_to_execution_result(row)?)
            })?
            .filter_map(|r| r.ok())
            .collect();

        Ok(results)
    }

    /// Import scripts from an export
    pub fn import(&self, export: &ScriptExport) -> ScriptStoreResult<Vec<CommandScript>> {
        let mut imported = Vec::new();

        for mut script in export.scripts.clone() {
            // Generate new IDs to avoid conflicts
            script.id = Uuid::new_v4();
            script.created_at = Utc::now();
            script.updated_at = Utc::now();
            script.version = 1;
            script.usage_count = 0;

            let created = self.create(&script)?;
            imported.push(created);
        }

        Ok(imported)
    }

    /// Export scripts matching the filter
    pub fn export(&self, filter: &ScriptFilter) -> ScriptStoreResult<ScriptExport> {
        let scripts = self.list(filter)?;
        Ok(ScriptExport::new(scripts))
    }

    /// Get all scripts (for export)
    pub fn get_all(&self) -> ScriptStoreResult<Vec<CommandScript>> {
        self.list(&ScriptFilter::default())
    }

    /// Bulk delete scripts by IDs
    pub fn bulk_delete(&self, ids: &[Uuid]) -> ScriptStoreResult<u64> {
        let conn = self.conn.lock().unwrap();
        let mut total_deleted = 0u64;

        for id in ids {
            let deleted = conn.execute(
                "DELETE FROM scripts WHERE id = ?1",
                params![id.to_string()],
            )?;
            total_deleted += deleted as u64;
        }

        Ok(total_deleted)
    }

    /// Search scripts by name or description
    pub fn search(&self, query: &str, limit: u64) -> ScriptStoreResult<Vec<CommandScript>> {
        let filter = ScriptFilter {
            name_contains: Some(query.to_string()),
            offset: 0,
            limit,
            ..Default::default()
        };
        self.list(&filter)
    }
}

// Helper function to convert a database row to CommandScript
fn row_to_script(row: &rusqlite::Row) -> rusqlite::Result<CommandScript> {
    let id_str: String = row.get(0)?;
    let name: String = row.get(1)?;
    let description: Option<String> = row.get(2)?;
    let script_type_str: String = row.get(3)?;
    let content: String = row.get(4)?;
    let parameters_json: String = row.get(5)?;
    let is_template: i32 = row.get(6)?;
    let is_favorite: i32 = row.get(7)?;
    let tags_json: String = row.get(8)?;
    let created_by: Option<String> = row.get(9)?;
    let created_at_str: String = row.get(10)?;
    let updated_at_str: String = row.get(11)?;
    let version: u32 = row.get(12)?;
    let usage_count: u64 = row.get(13)?;
    let selected_options_json: String = row.get(14)?;

    let id = Uuid::parse_str(&id_str).unwrap_or_else(|_| Uuid::new_v4());
    let script_type = ScriptType::from_str(&script_type_str).unwrap_or_default();
    let parameters: Vec<ScriptParameter> = serde_json::from_str(&parameters_json).unwrap_or_default();
    let tags: Vec<String> = serde_json::from_str(&tags_json).unwrap_or_default();
    let selected_options: std::collections::HashMap<String, String> =
        serde_json::from_str(&selected_options_json).unwrap_or_default();
    let created_at = DateTime::parse_from_rfc3339(&created_at_str)
        .map(|dt| dt.with_timezone(&Utc))
        .unwrap_or_else(|_| Utc::now());
    let updated_at = DateTime::parse_from_rfc3339(&updated_at_str)
        .map(|dt| dt.with_timezone(&Utc))
        .unwrap_or_else(|_| Utc::now());

    Ok(CommandScript {
        id,
        name,
        description,
        script_type,
        content,
        parameters,
        is_template: is_template != 0,
        is_favorite: is_favorite != 0,
        tags,
        created_by,
        created_at,
        updated_at,
        version,
        usage_count,
        selected_options,
    })
}

// Helper function to convert a database row to ScriptExecutionResult
fn row_to_execution_result(row: &rusqlite::Row) -> rusqlite::Result<ScriptExecutionResult> {
    let id_str: String = row.get(0)?;
    let script_id_str: String = row.get(1)?;
    let exit_code: Option<i32> = row.get(2)?;
    let stdout: String = row.get(3)?;
    let stderr: String = row.get(4)?;
    let duration_ms: i64 = row.get(5)?;
    let success: i32 = row.get(6)?;
    let executed_at_str: String = row.get(7)?;
    let error: Option<String> = row.get(8)?;

    let execution_id = Uuid::parse_str(&id_str).unwrap_or_else(|_| Uuid::new_v4());
    let script_id = Uuid::parse_str(&script_id_str).unwrap_or_else(|_| Uuid::new_v4());
    let executed_at = DateTime::parse_from_rfc3339(&executed_at_str)
        .map(|dt| dt.with_timezone(&Utc))
        .unwrap_or_else(|_| Utc::now());

    Ok(ScriptExecutionResult {
        execution_id,
        script_id,
        exit_code,
        stdout,
        stderr,
        duration_ms: duration_ms as u64,
        success: success != 0,
        executed_at,
        error,
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    fn create_test_store() -> ScriptStore {
        ScriptStore::in_memory().unwrap()
    }

    #[test]
    fn test_create_and_get() {
        let store = create_test_store();
        let script = CommandScript::new("Test Script", "echo hello").with_type(ScriptType::Shell);

        let created = store.create(&script).unwrap();
        assert_eq!(created.name, "Test Script");

        let retrieved = store.get(created.id).unwrap();
        assert_eq!(retrieved.name, "Test Script");
        assert_eq!(retrieved.content, "echo hello");
    }

    #[test]
    fn test_create_and_get_with_parameters() {
        let store = create_test_store();
        let script = CommandScript::new_template("Template", "echo {{name}}")
            .with_parameter(ScriptParameter::required_string("name"));

        let created = store.create(&script).unwrap();
        let retrieved = store.get(created.id).unwrap();

        assert!(retrieved.is_template);
        assert_eq!(retrieved.parameters.len(), 1);
        assert_eq!(retrieved.parameters[0].name, "name");
    }

    #[test]
    fn test_update() {
        let store = create_test_store();
        let mut script = CommandScript::new("Original", "echo original");
        store.create(&script).unwrap();

        script.name = "Updated".to_string();
        script.content = "echo updated".to_string();
        let updated = store.update(&script).unwrap();

        assert_eq!(updated.name, "Updated");
        assert_eq!(updated.version, 2);
    }

    #[test]
    fn test_delete() {
        let store = create_test_store();
        let script = CommandScript::new("To Delete", "echo delete me");
        let created = store.create(&script).unwrap();

        store.delete(created.id).unwrap();

        assert!(store.get(created.id).is_err());
    }

    #[test]
    fn test_delete_not_found() {
        let store = create_test_store();
        let result = store.delete(Uuid::new_v4());
        assert!(result.is_err());
    }

    #[test]
    fn test_list_with_filter() {
        let store = create_test_store();

        // Create scripts with different types and tags
        let script1 = CommandScript::new("Script 1", "echo 1")
            .with_type(ScriptType::Shell)
            .with_tag("test");
        let script2 = CommandScript::new("Script 2", "echo 2")
            .with_type(ScriptType::Powershell)
            .with_tag("test");
        let script3 = CommandScript::new("Other", "echo 3")
            .with_type(ScriptType::Python)
            .with_tag("other");

        store.create(&script1).unwrap();
        store.create(&script2).unwrap();
        store.create(&script3).unwrap();

        // Filter by type
        let shell_scripts = store.list(&ScriptFilter {
            script_type: Some(ScriptType::Shell),
            ..Default::default()
        }).unwrap();
        assert_eq!(shell_scripts.len(), 1);

        // Filter by tag
        let test_scripts = store.list(&ScriptFilter {
            tags: Some(vec!["test".to_string()]),
            ..Default::default()
        }).unwrap();
        assert_eq!(test_scripts.len(), 2);

        // Filter by name
        let filtered = store.list(&ScriptFilter {
            name_contains: Some("Script".to_string()),
            ..Default::default()
        }).unwrap();
        assert_eq!(filtered.len(), 2);
    }

    #[test]
    fn test_toggle_favorite() {
        let store = create_test_store();
        let script = CommandScript::new("Fav Test", "echo fav");
        let created = store.create(&script).unwrap();

        assert!(!created.is_favorite);

        let toggled = store.toggle_favorite(created.id).unwrap();
        assert!(toggled.is_favorite);

        let toggled_again = store.toggle_favorite(created.id).unwrap();
        assert!(!toggled_again.is_favorite);
    }

    #[test]
    fn test_increment_usage() {
        let store = create_test_store();
        let script = CommandScript::new("Usage Test", "echo usage");
        let created = store.create(&script).unwrap();

        assert_eq!(created.usage_count, 0);

        store.increment_usage(created.id).unwrap();
        let after_one = store.get(created.id).unwrap();
        assert_eq!(after_one.usage_count, 1);

        store.increment_usage(created.id).unwrap();
        let after_two = store.get(created.id).unwrap();
        assert_eq!(after_two.usage_count, 2);
    }

    #[test]
    fn test_stats() {
        let store = create_test_store();

        let script1 = CommandScript::new("Script 1", "echo 1").with_tag("test");
        let script2 = CommandScript::new_template("Template", "echo 2");
        let mut script3 = CommandScript::new("Fav", "echo 3");
        script3.is_favorite = true;

        store.create(&script1).unwrap();
        store.create(&script2).unwrap();
        store.create(&script3).unwrap();

        let stats = store.stats().unwrap();
        assert_eq!(stats.total_scripts, 3);
        assert_eq!(stats.template_count, 1);
        assert_eq!(stats.favorite_count, 1);
    }

    #[test]
    fn test_save_and_get_execution() {
        let store = create_test_store();
        let script = CommandScript::new("Exec Test", "echo exec");
        let created = store.create(&script).unwrap();

        let result = ScriptExecutionResult::new(created.id)
            .success(0, "hello", "", 100);

        store.save_execution(&result).unwrap();

        let executions = store.get_executions(created.id, 10).unwrap();
        assert_eq!(executions.len(), 1);
        assert!(executions[0].success);
        assert_eq!(executions[0].stdout, "hello");
    }

    #[test]
    fn test_import_export() {
        let store = create_test_store();

        let script1 = CommandScript::new("Script 1", "echo 1")
            .with_type(ScriptType::Shell)
            .with_tag("test");
        let script2 = CommandScript::new_template("Template", "echo {{name}}")
            .with_parameter(ScriptParameter::required_string("name"));

        store.create(&script1).unwrap();
        store.create(&script2).unwrap();

        // Export
        let export = store.export(&ScriptFilter::default()).unwrap();
        assert_eq!(export.scripts.len(), 2);

        // Serialize to JSON
        let json = export.to_json().unwrap();

        // Import to a new store
        let store2 = create_test_store();
        let imported_export = ScriptExport::from_json(&json).unwrap();
        let imported = store2.import(&imported_export).unwrap();

        assert_eq!(imported.len(), 2);
        // Verify IDs are different (new IDs generated)
        assert_ne!(imported[0].id, script1.id);
        assert_ne!(imported[1].id, script2.id);
        // But names should match
        assert_eq!(imported[0].name, "Script 1");
        assert_eq!(imported[1].name, "Template");
    }

    #[test]
    fn test_search() {
        let store = create_test_store();

        let script1 = CommandScript::new("Check CPU Usage", "top");
        let script2 = CommandScript::new("Check Memory", "free -m");
        let script3 = CommandScript::new("List Files", "ls");

        store.create(&script1).unwrap();
        store.create(&script2).unwrap();
        store.create(&script3).unwrap();

        let results = store.search("check", 10).unwrap();
        assert_eq!(results.len(), 2);

        let results = store.search("CPU", 10).unwrap();
        assert_eq!(results.len(), 1);
    }

    #[test]
    fn test_bulk_delete() {
        let store = create_test_store();

        let script1 = CommandScript::new("Delete Me 1", "echo 1");
        let script2 = CommandScript::new("Delete Me 2", "echo 2");
        let script3 = CommandScript::new("Keep Me", "echo 3");

        let created1 = store.create(&script1).unwrap();
        let created2 = store.create(&script2).unwrap();
        store.create(&script3).unwrap();

        let deleted = store.bulk_delete(&[created1.id, created2.id]).unwrap();
        assert_eq!(deleted, 2);

        let remaining = store.list(&ScriptFilter::default()).unwrap();
        assert_eq!(remaining.len(), 1);
        assert_eq!(remaining[0].name, "Keep Me");
    }
}
