//! Batch Operations Module
//!
//! This module provides batch task management, target selection, and execution policies
//! for command script operations across multiple stations.

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use uuid::Uuid;

use crate::scripts::{CommandScript, ScriptExecutionResult};

/// Batch task status enumeration.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize, PartialOrd, Ord)]
#[serde(rename_all = "snake_case")]
pub enum BatchTaskStatus {
    /// Task is created but not started
    Pending,
    /// Task is currently executing
    Running,
    /// Task completed successfully
    Completed,
    /// Task completed with some failures
    PartialFailure,
    /// Task failed completely
    Failed,
    /// Task was cancelled
    Cancelled,
}

impl Default for BatchTaskStatus {
    fn default() -> Self {
        Self::Pending
    }
}

impl std::fmt::Display for BatchTaskStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            BatchTaskStatus::Pending => write!(f, "pending"),
            BatchTaskStatus::Running => write!(f, "running"),
            BatchTaskStatus::Completed => write!(f, "completed"),
            BatchTaskStatus::PartialFailure => write!(f, "partial_failure"),
            BatchTaskStatus::Failed => write!(f, "failed"),
            BatchTaskStatus::Cancelled => write!(f, "cancelled"),
        }
    }
}

/// Target selector for batch operations.
/// Defines which stations should be targeted by a batch task.
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(tag = "type", rename_all = "snake_case")]
pub enum TargetSelector {
    /// Select all stations
    All,
    /// Select stations by exact ID match
    ByIds { ids: Vec<String> },
    /// Select stations matching a pattern (wildcard support)
    ByPattern { pattern: String },
    /// Select stations by tag
    ByTag { tag: String },
    /// Select stations by station group
    ByGroup { group: String },
}

impl Default for TargetSelector {
    fn default() -> Self {
        Self::All
    }
}

impl TargetSelector {
    /// Parse a target selector from a string representation.
    /// Formats:
    /// - "all" -> TargetSelector::All
    /// - "id1,id2,id3" -> TargetSelector::ByIds
    /// - "@tagname" -> TargetSelector::ByTag
    /// - "#groupname" -> TargetSelector::ByGroup
    /// - "pattern*" -> TargetSelector::ByPattern
    pub fn parse(s: &str) -> Self {
        let s = s.trim();
        if s.is_empty() || s.eq_ignore_ascii_case("all") {
            return TargetSelector::All;
        }

        if s.starts_with('@') {
            return TargetSelector::ByTag {
                tag: s[1..].to_string(),
            };
        }

        if s.starts_with('#') {
            return TargetSelector::ByGroup {
                group: s[1..].to_string(),
            };
        }

        if s.ends_with('*') {
            return TargetSelector::ByPattern {
                pattern: s.trim_end_matches('*').to_string(),
            };
        }

        // Check if it contains commas (list of IDs)
        if s.contains(',') {
            let ids: Vec<String> = s
                .split(',')
                .map(|part| part.trim().to_string())
                .filter(|part| !part.is_empty())
                .collect();
            if !ids.is_empty() {
                return TargetSelector::ByIds { ids };
            }
        }

        // Single ID
        TargetSelector::ByIds {
            ids: vec![s.to_string()],
        }
    }

    /// Check if a station ID matches this selector
    pub fn matches(&self, station_id: &str, tags: &[String], groups: &[String]) -> bool {
        match self {
            TargetSelector::All => true,
            TargetSelector::ByIds { ids } => ids.iter().any(|id| id == station_id),
            TargetSelector::ByPattern { pattern } => station_id.starts_with(pattern),
            TargetSelector::ByTag { tag } => tags.iter().any(|t| t == tag),
            TargetSelector::ByGroup { group } => groups.iter().any(|g| g == group),
        }
    }
}

/// Execution policy for batch tasks.
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum ExecutionPolicy {
    /// Execute on all targets simultaneously (parallel)
    Parallel,
    /// Execute sequentially, one target at a time
    Sequential,
    /// Execute with a concurrency limit
    LimitedParallel { max_concurrency: u32 },
    /// Execute in batches with delay between batches
    BatchWithDelay { batch_size: u32, delay_ms: u64 },
}

impl Default for ExecutionPolicy {
    fn default() -> Self {
        Self::Parallel
    }
}

impl ExecutionPolicy {
    /// Get the maximum number of concurrent executions
    pub fn max_concurrency(&self) -> Option<u32> {
        match self {
            ExecutionPolicy::Parallel => None, // unlimited
            ExecutionPolicy::Sequential => Some(1),
            ExecutionPolicy::LimitedParallel { max_concurrency } => Some(*max_concurrency),
            ExecutionPolicy::BatchWithDelay { batch_size, .. } => Some(*batch_size),
        }
    }
}

/// A parameter value for a batch task execution.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct BatchParameterValue {
    /// Parameter name
    pub name: String,
    /// Parameter value (may be different per target)
    pub value: String,
}

/// A single execution item within a batch task.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct BatchExecutionItem {
    /// Target station ID
    pub station_id: String,
    /// Script ID to execute
    pub script_id: Uuid,
    /// Parameter values for this execution
    pub parameters: Vec<BatchParameterValue>,
    /// Execution result (set after execution)
    #[serde(default)]
    pub result: Option<ScriptExecutionResult>,
    /// Execution status
    #[serde(default)]
    pub status: BatchExecutionStatus,
}

/// Status of a single execution item.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum BatchExecutionStatus {
    Pending,
    Running,
    Success,
    Failed,
    Skipped,
}

impl Default for BatchExecutionStatus {
    fn default() -> Self {
        Self::Pending
    }
}

/// A batch task that coordinates execution of scripts across multiple stations.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct BatchTask {
    /// Unique identifier
    pub id: Uuid,
    /// Task name
    pub name: String,
    /// Task description
    pub description: Option<String>,
    /// Target selector
    #[serde(default)]
    pub target_selector: TargetSelector,
    /// Execution policy
    #[serde(default)]
    pub execution_policy: ExecutionPolicy,
    /// Script to execute
    pub script_id: Uuid,
    /// Script content snapshot at creation time
    pub script_content: String,
    /// Default parameters for all executions
    #[serde(default)]
    pub default_parameters: Vec<BatchParameterValue>,
    /// Execution items (one per target)
    #[serde(default)]
    pub execution_items: Vec<BatchExecutionItem>,
    /// Task status
    #[serde(default)]
    pub status: BatchTaskStatus,
    /// Progress: number of completed items
    #[serde(default)]
    pub progress: u32,
    /// Total number of items
    #[serde(default)]
    pub total_items: u32,
    /// Creation timestamp
    pub created_at: DateTime<Utc>,
    /// Last update timestamp
    pub updated_at: DateTime<Utc>,
    /// Started at timestamp (None if not started)
    pub started_at: Option<DateTime<Utc>>,
    /// Completed at timestamp (None if not completed)
    pub completed_at: Option<DateTime<Utc>>,
    /// Creator identifier
    pub created_by: Option<String>,
}

impl BatchTask {
    /// Create a new batch task
    pub fn new(name: impl Into<String>, script: &CommandScript) -> Self {
        let now = Utc::now();
        Self {
            id: Uuid::new_v4(),
            name: name.into(),
            description: None,
            target_selector: TargetSelector::default(),
            execution_policy: ExecutionPolicy::default(),
            script_id: script.id,
            script_content: script.content.clone(),
            default_parameters: Vec::new(),
            execution_items: Vec::new(),
            status: BatchTaskStatus::Pending,
            progress: 0,
            total_items: 0,
            created_at: now,
            updated_at: now,
            started_at: None,
            completed_at: None,
            created_by: None,
        }
    }

    /// Set the description
    pub fn with_description(mut self, description: impl Into<String>) -> Self {
        self.description = Some(description.into());
        self
    }

    /// Set the target selector
    pub fn with_target_selector(mut self, selector: TargetSelector) -> Self {
        self.target_selector = selector;
        self
    }

    /// Set the execution policy
    pub fn with_execution_policy(mut self, policy: ExecutionPolicy) -> Self {
        self.execution_policy = policy;
        self
    }

    /// Add a default parameter
    pub fn with_default_parameter(mut self, name: impl Into<String>, value: impl Into<String>) -> Self {
        self.default_parameters.push(BatchParameterValue {
            name: name.into(),
            value: value.into(),
        });
        self
    }

    /// Set the creator
    pub fn created_by(mut self, creator: impl Into<String>) -> Self {
        self.created_by = Some(creator.into());
        self
    }

    /// Calculate the completion percentage
    pub fn completion_percentage(&self) -> f64 {
        if self.total_items == 0 {
            return 0.0;
        }
        (self.progress as f64 / self.total_items as f64) * 100.0
    }

    /// Check if the task is complete (finished or cancelled)
    pub fn is_finished(&self) -> bool {
        matches!(
            self.status,
            BatchTaskStatus::Completed
                | BatchTaskStatus::PartialFailure
                | BatchTaskStatus::Failed
                | BatchTaskStatus::Cancelled
        )
    }

    /// Update the status based on execution items
    pub fn update_status(&mut self) {
        let total = self.execution_items.len() as u32;
        let completed = self
            .execution_items
            .iter()
            .filter(|item| matches!(item.status, BatchExecutionStatus::Success | BatchExecutionStatus::Failed))
            .count() as u32;

        self.progress = completed;
        self.total_items = total;
        self.updated_at = Utc::now();

        if completed == 0 && total == 0 {
            self.status = BatchTaskStatus::Pending;
        } else if completed == total {
            let has_failures = self
                .execution_items
                .iter()
                .any(|item| item.status == BatchExecutionStatus::Failed);
            self.status = if has_failures {
                BatchTaskStatus::PartialFailure
            } else {
                BatchTaskStatus::Completed
            };
            self.completed_at = Some(Utc::now());
        } else if completed < total {
            self.status = BatchTaskStatus::Running;
        }
    }
}

/// Filter options for querying batch tasks.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct BatchTaskFilter {
    /// Filter by status
    pub status: Option<BatchTaskStatus>,
    /// Filter by target selector type
    pub target_type: Option<String>,
    /// Filter by creator
    pub created_by: Option<String>,
    /// Sort by field
    #[serde(default)]
    pub sort_by: BatchTaskSortField,
    /// Sort ascending or descending
    #[serde(default = "default_sort_desc")]
    pub sort_desc: bool,
    /// Pagination offset
    #[serde(default)]
    pub offset: u64,
    /// Pagination limit
    #[serde(default = "default_limit")]
    pub limit: u64,
}

fn default_sort_desc() -> bool {
    true
}

fn default_limit() -> u64 {
    50
}

/// Fields that can be used for sorting batch tasks.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum BatchTaskSortField {
    CreatedAt,
    UpdatedAt,
    Name,
    Status,
    Progress,
}

impl Default for BatchTaskSortField {
    fn default() -> Self {
        Self::CreatedAt
    }
}

/// Statistics about batch task executions.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct BatchTaskStats {
    /// Total number of tasks
    pub total_tasks: u64,
    /// Number of pending tasks
    pub pending_tasks: u64,
    /// Number of running tasks
    pub running_tasks: u64,
    /// Number of completed tasks
    pub completed_tasks: u64,
    /// Number of failed tasks
    pub failed_tasks: u64,
    /// Total executions across all tasks
    pub total_executions: u64,
    /// Successful executions
    pub successful_executions: u64,
    /// Failed executions
    pub failed_executions: u64,
}

/// Batch task execution engine.
///
/// This handles the actual execution of batch tasks according to their execution policy.
pub struct BatchExecutor;

impl BatchExecutor {
    /// Determine the execution order based on the execution policy.
    ///
    /// Returns indices into the execution items in the order they should be executed.
    pub fn determine_execution_order(
        items: &[BatchExecutionItem],
        policy: &ExecutionPolicy,
    ) -> Vec<Vec<usize>> {
        match policy {
            ExecutionPolicy::Parallel => {
                // All items in one batch
                vec![items.iter().enumerate().map(|(i, _)| i).collect()]
            }
            ExecutionPolicy::Sequential => {
                // Each item in its own batch
                items
                    .iter()
                    .enumerate()
                    .map(|(i, _)| vec![i])
                    .collect()
            }
            ExecutionPolicy::LimitedParallel { max_concurrency } => {
                // Split into batches of max_concurrency
                let mut batches = Vec::new();
                let mut current_batch = Vec::new();
                for (i, _) in items.iter().enumerate() {
                    current_batch.push(i);
                    if current_batch.len() >= *max_concurrency as usize {
                        batches.push(current_batch);
                        current_batch = Vec::new();
                    }
                }
                if !current_batch.is_empty() {
                    batches.push(current_batch);
                }
                batches
            }
            ExecutionPolicy::BatchWithDelay { batch_size, .. } => {
                // Split into batches of batch_size
                let mut batches = Vec::new();
                let mut current_batch = Vec::new();
                for (i, _) in items.iter().enumerate() {
                    current_batch.push(i);
                    if current_batch.len() >= *batch_size as usize {
                        batches.push(current_batch);
                        current_batch = Vec::new();
                    }
                }
                if !current_batch.is_empty() {
                    batches.push(current_batch);
                }
                batches
            }
        }
    }

    /// Get the delay between batches (for BatchWithDelay policy)
    pub fn get_batch_delay_ms(policy: &ExecutionPolicy) -> u64 {
        match policy {
            ExecutionPolicy::BatchWithDelay { delay_ms, .. } => *delay_ms,
            _ => 0,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_target_selector_parse() {
        assert!(matches!(TargetSelector::parse("all"), TargetSelector::All));
        assert!(matches!(TargetSelector::parse(""), TargetSelector::All));
        assert!(matches!(TargetSelector::parse("ALL"), TargetSelector::All));

        if let TargetSelector::ByIds { ids } = TargetSelector::parse("id1,id2,id3") {
            assert_eq!(ids, vec!["id1", "id2", "id3"]);
        } else {
            panic!("Expected ByIds");
        }

        if let TargetSelector::ByTag { tag } = TargetSelector::parse("@production") {
            assert_eq!(tag, "production");
        } else {
            panic!("Expected ByTag");
        }

        if let TargetSelector::ByGroup { group } = TargetSelector::parse("#backend") {
            assert_eq!(group, "backend");
        } else {
            panic!("Expected ByGroup");
        }

        if let TargetSelector::ByPattern { pattern } = TargetSelector::parse("station-*") {
            assert_eq!(pattern, "station-");
        } else {
            panic!("Expected ByPattern");
        }

        if let TargetSelector::ByIds { ids } = TargetSelector::parse("single-id") {
            assert_eq!(ids, vec!["single-id"]);
        } else {
            panic!("Expected single ByIds");
        }
    }

    #[test]
    fn test_target_selector_matches() {
        let tags = vec!["production".to_string(), "linux".to_string()];
        let groups = vec!["backend".to_string(), "frontend".to_string()];

        assert!(TargetSelector::All.matches("any-id", &tags, &groups));

        assert!(TargetSelector::ByIds {
            ids: vec!["id1".to_string(), "id2".to_string()]
        }
        .matches("id1", &tags, &groups));

        assert!(!TargetSelector::ByIds {
            ids: vec!["id1".to_string(), "id2".to_string()]
        }
        .matches("id3", &tags, &groups));

        assert!(TargetSelector::ByPattern {
            pattern: "station-".to_string()
        }
        .matches("station-001", &tags, &groups));

        assert!(TargetSelector::ByTag {
            tag: "production".to_string()
        }
        .matches("any-id", &tags, &groups));

        assert!(TargetSelector::ByGroup {
            group: "backend".to_string()
        }
        .matches("any-id", &tags, &groups));
    }

    #[test]
    fn test_execution_policy_max_concurrency() {
        assert!(ExecutionPolicy::Parallel.max_concurrency().is_none());
        assert_eq!(ExecutionPolicy::Sequential.max_concurrency(), Some(1));
        assert_eq!(
            ExecutionPolicy::LimitedParallel { max_concurrency: 5 }.max_concurrency(),
            Some(5)
        );
        assert_eq!(
            ExecutionPolicy::BatchWithDelay {
                batch_size: 10,
                delay_ms: 1000
            }
            .max_concurrency(),
            Some(10)
        );
    }

    #[test]
    fn test_batch_executor_order() {
        let items: Vec<BatchExecutionItem> = (0..5)
            .map(|i| BatchExecutionItem {
                station_id: format!("station-{}", i),
                script_id: Uuid::new_v4(),
                parameters: Vec::new(),
                result: None,
                status: BatchExecutionStatus::Pending,
            })
            .collect();

        // Parallel - all in one batch
        let batches = BatchExecutor::determine_execution_order(&items, &ExecutionPolicy::Parallel);
        assert_eq!(batches.len(), 1);
        assert_eq!(batches[0], vec![0, 1, 2, 3, 4]);

        // Sequential - each in own batch
        let batches = BatchExecutor::determine_execution_order(&items, &ExecutionPolicy::Sequential);
        assert_eq!(batches.len(), 5);
        assert_eq!(batches[0], vec![0]);
        assert_eq!(batches[4], vec![4]);

        // LimitedParallel with max 2
        let batches = BatchExecutor::determine_execution_order(
            &items,
            &ExecutionPolicy::LimitedParallel { max_concurrency: 2 },
        );
        assert_eq!(batches.len(), 3);
        assert_eq!(batches[0], vec![0, 1]);
        assert_eq!(batches[1], vec![2, 3]);
        assert_eq!(batches[2], vec![4]);

        // BatchWithDelay with batch_size 2
        let batches = BatchExecutor::determine_execution_order(
            &items,
            &ExecutionPolicy::BatchWithDelay {
                batch_size: 2,
                delay_ms: 1000,
            },
        );
        assert_eq!(batches.len(), 3);
        assert_eq!(batches[0], vec![0, 1]);
        assert_eq!(batches[2], vec![4]);
    }

    #[test]
    fn test_batch_task_creation() {
        let script = CommandScript::new("test script", "echo hello");
        let task = BatchTask::new("Test Task", &script);
        
        assert_eq!(task.name, "Test Task");
        assert_eq!(task.script_id, script.id);
        assert_eq!(task.script_content, "echo hello");
        assert_eq!(task.status, BatchTaskStatus::Pending);
        assert_eq!(task.progress, 0);
        assert_eq!(task.total_items, 0);
    }

    #[test]
    fn test_batch_task_update_status() {
        let script = CommandScript::new("test script", "echo hello");
        let mut task = BatchTask::new("Test Task", &script);
        
        // Add execution items
        task.execution_items.push(BatchExecutionItem {
            station_id: "station-1".to_string(),
            script_id: script.id,
            parameters: Vec::new(),
            result: None,
            status: BatchExecutionStatus::Success,
        });
        task.execution_items.push(BatchExecutionItem {
            station_id: "station-2".to_string(),
            script_id: script.id,
            parameters: Vec::new(),
            result: None,
            status: BatchExecutionStatus::Failed,
        });

        task.update_status();
        assert_eq!(task.status, BatchTaskStatus::PartialFailure);
        assert_eq!(task.progress, 2);
        assert_eq!(task.total_items, 2);
        assert!(task.completed_at.is_some());
    }

    #[test]
    fn test_batch_task_completion_percentage() {
        let script = CommandScript::new("test script", "echo hello");
        let mut task = BatchTask::new("Test Task", &script);
        
        assert_eq!(task.completion_percentage(), 0.0);

        task.total_items = 4;
        task.progress = 2;
        assert_eq!(task.completion_percentage(), 50.0);

        task.progress = 4;
        assert_eq!(task.completion_percentage(), 100.0);
    }
}
