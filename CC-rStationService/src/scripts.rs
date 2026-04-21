//! Command Script Data Structures
//!
//! This module defines the core data structures for the command script management system,
//! including scripts, parameters, and execution results.

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use uuid::Uuid;

/// Script type enumeration representing supported scripting languages.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum ScriptType {
    /// Shell script (bash, sh)
    Shell,
    /// PowerShell script
    Powershell,
    /// Python script
    Python,
    /// Lua script
    Lua,
}

impl Default for ScriptType {
    fn default() -> Self {
        Self::Shell
    }
}

impl std::fmt::Display for ScriptType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ScriptType::Shell => write!(f, "shell"),
            ScriptType::Powershell => write!(f, "powershell"),
            ScriptType::Python => write!(f, "python"),
            ScriptType::Lua => write!(f, "lua"),
        }
    }
}

impl ScriptType {
    /// Parse from string, case-insensitive
    pub fn from_str(s: &str) -> Option<Self> {
        match s.to_lowercase().as_str() {
            "shell" | "sh" | "bash" => Some(ScriptType::Shell),
            "powershell" | "ps" | "ps1" | "pwsh" => Some(ScriptType::Powershell),
            "python" | "py" | "python3" => Some(ScriptType::Python),
            "lua" => Some(ScriptType::Lua),
            _ => None,
        }
    }

    /// Get the file extension for this script type
    pub fn file_extension(&self) -> &'static str {
        match self {
            ScriptType::Shell => "sh",
            ScriptType::Powershell => "ps1",
            ScriptType::Python => "py",
            ScriptType::Lua => "lua",
        }
    }

    /// Get the interpreter/command for this script type
    pub fn interpreter(&self) -> &'static str {
        match self {
            ScriptType::Shell => "sh",
            ScriptType::Powershell => "pwsh",
            ScriptType::Python => "python3",
            ScriptType::Lua => "lua",
        }
    }
}

/// Parameter type for script parameters.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum ParameterType {
    String,
    Number,
    Boolean,
    Select,
}

impl Default for ParameterType {
    fn default() -> Self {
        Self::String
    }
}

impl std::fmt::Display for ParameterType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ParameterType::String => write!(f, "string"),
            ParameterType::Number => write!(f, "number"),
            ParameterType::Boolean => write!(f, "boolean"),
            ParameterType::Select => write!(f, "select"),
        }
    }
}

/// A parameter definition for a script template.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ScriptParameter {
    /// Parameter name (used as placeholder key)
    pub name: String,
    /// Parameter type
    #[serde(default)]
    pub param_type: ParameterType,
    /// Display label
    pub label: Option<String>,
    /// Description of the parameter
    pub description: Option<String>,
    /// Default value (as string)
    #[serde(default)]
    pub default_value: Option<String>,
    /// Whether this parameter is required
    #[serde(default)]
    pub required: bool,
    /// Validation regex pattern (optional)
    pub validation: Option<String>,
    /// For Select type: allowed values
    #[serde(default)]
    pub options: Vec<String>,
}

impl ScriptParameter {
    /// Create a new required string parameter
    pub fn required_string(name: impl Into<String>) -> Self {
        Self {
            name: name.into(),
            param_type: ParameterType::String,
            label: None,
            description: None,
            default_value: None,
            required: true,
            validation: None,
            options: Vec::new(),
        }
    }

    /// Create a new optional string parameter with a default value
    pub fn optional_string(name: impl Into<String>, default: impl Into<String>) -> Self {
        Self {
            name: name.into(),
            param_type: ParameterType::String,
            label: None,
            description: None,
            default_value: Some(default.into()),
            required: false,
            validation: None,
            options: Vec::new(),
        }
    }

    /// Create a new select parameter with options
    pub fn select(
        name: impl Into<String>,
        options: Vec<String>,
        default: Option<String>,
    ) -> Self {
        Self {
            name: name.into(),
            param_type: ParameterType::Select,
            label: None,
            description: None,
            default_value: default,
            required: false,
            validation: None,
            options,
        }
    }

    /// Set the label
    pub fn with_label(mut self, label: impl Into<String>) -> Self {
        self.label = Some(label.into());
        self
    }

    /// Set the description
    pub fn with_description(mut self, description: impl Into<String>) -> Self {
        self.description = Some(description.into());
        self
    }
}

/// A command script definition.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CommandScript {
    /// Unique identifier
    pub id: Uuid,
    /// Script name
    pub name: String,
    /// Script description
    pub description: Option<String>,
    /// Script type
    #[serde(default)]
    pub script_type: ScriptType,
    /// Script content/source code
    pub content: String,
    /// Parameter definitions (for template scripts)
    #[serde(default)]
    pub parameters: Vec<ScriptParameter>,
    /// Whether this is a template
    #[serde(default)]
    pub is_template: bool,
    /// Whether this script is marked as favorite
    #[serde(default)]
    pub is_favorite: bool,
    /// Tags for categorization
    #[serde(default)]
    pub tags: Vec<String>,
    /// Creator identifier
    pub created_by: Option<String>,
    /// Creation timestamp
    pub created_at: DateTime<Utc>,
    /// Last update timestamp
    pub updated_at: DateTime<Utc>,
    /// Version number
    #[serde(default = "default_version")]
    pub version: u32,
    /// Usage count
    #[serde(default)]
    pub usage_count: u64,
    /// For Select type parameters, selected values
    #[serde(default)]
    pub selected_options: std::collections::HashMap<String, String>,
}

fn default_version() -> u32 {
    1
}

impl CommandScript {
    /// Create a new script with the given name and content
    pub fn new(name: impl Into<String>, content: impl Into<String>) -> Self {
        let now = Utc::now();
        Self {
            id: Uuid::new_v4(),
            name: name.into(),
            description: None,
            script_type: ScriptType::default(),
            content: content.into(),
            parameters: Vec::new(),
            is_template: false,
            is_favorite: false,
            tags: Vec::new(),
            created_by: None,
            created_at: now,
            updated_at: now,
            version: 1,
            usage_count: 0,
            selected_options: std::collections::HashMap::new(),
        }
    }

    /// Create a new template script
    pub fn new_template(name: impl Into<String>, content: impl Into<String>) -> Self {
        let mut script = Self::new(name, content);
        script.is_template = true;
        script
    }

    /// Set the script type
    pub fn with_type(mut self, script_type: ScriptType) -> Self {
        self.script_type = script_type;
        self
    }

    /// Set the description
    pub fn with_description(mut self, description: impl Into<String>) -> Self {
        self.description = Some(description.into());
        self
    }

    /// Add a tag
    pub fn with_tag(mut self, tag: impl Into<String>) -> Self {
        self.tags.push(tag.into());
        self
    }

    /// Add multiple tags
    pub fn with_tags(mut self, tags: impl IntoIterator<Item = String>) -> Self {
        self.tags.extend(tags);
        self
    }

    /// Add a parameter
    pub fn with_parameter(mut self, param: ScriptParameter) -> Self {
        self.parameters.push(param);
        self
    }

    /// Set the creator
    pub fn created_by(mut self, creator: impl Into<String>) -> Self {
        self.created_by = Some(creator.into());
        self
    }

    /// Increment the usage count
    pub fn increment_usage(&mut self) {
        self.usage_count += 1;
        self.updated_at = Utc::now();
    }

    /// Toggle favorite status
    pub fn toggle_favorite(&mut self) {
        self.is_favorite = !self.is_favorite;
        self.updated_at = Utc::now();
    }

    /// Update the script content and increment version
    pub fn update_content(&mut self, new_content: impl Into<String>) {
        self.content = new_content.into();
        self.version += 1;
        self.updated_at = Utc::now();
    }
}

/// Result of a script execution.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScriptExecutionResult {
    /// Execution ID
    pub execution_id: Uuid,
    /// Script ID that was executed
    pub script_id: Uuid,
    /// Exit code
    pub exit_code: Option<i32>,
    /// Standard output
    pub stdout: String,
    /// Standard error
    pub stderr: String,
    /// Execution duration in milliseconds
    pub duration_ms: u64,
    /// Whether execution was successful
    pub success: bool,
    /// Execution timestamp
    pub executed_at: DateTime<Utc>,
    /// Error message if failed
    pub error: Option<String>,
}

impl ScriptExecutionResult {
    /// Create a new execution result
    pub fn new(script_id: Uuid) -> Self {
        Self {
            execution_id: Uuid::new_v4(),
            script_id,
            exit_code: None,
            stdout: String::new(),
            stderr: String::new(),
            duration_ms: 0,
            success: false,
            executed_at: Utc::now(),
            error: None,
        }
    }

    /// Mark as successful with the given output
    pub fn success(
        mut self,
        exit_code: i32,
        stdout: impl Into<String>,
        stderr: impl Into<String>,
        duration_ms: u64,
    ) -> Self {
        self.exit_code = Some(exit_code);
        self.stdout = stdout.into();
        self.stderr = stderr.into();
        self.duration_ms = duration_ms;
        self.success = exit_code == 0;
        self
    }

    /// Mark as failed with error message
    pub fn failed(mut self, error: impl Into<String>) -> Self {
        self.success = false;
        self.error = Some(error.into());
        self
    }
}

/// Filter options for querying scripts.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ScriptFilter {
    /// Filter by name (partial match)
    pub name_contains: Option<String>,
    /// Filter by script type
    pub script_type: Option<ScriptType>,
    /// Filter by tags (any match)
    pub tags: Option<Vec<String>>,
    /// Filter favorites only
    pub favorites_only: Option<bool>,
    /// Filter templates only
    pub templates_only: Option<bool>,
    /// Filter by creator
    pub created_by: Option<String>,
    /// Sort by field
    #[serde(default)]
    pub sort_by: ScriptSortField,
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
    false
}

fn default_limit() -> u64 {
    50
}

/// Fields that can be used for sorting.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum ScriptSortField {
    Name,
    CreatedAt,
    UpdatedAt,
    UsageCount,
    Version,
}

impl Default for ScriptSortField {
    fn default() -> Self {
        Self::UpdatedAt
    }
}

/// Statistics about script usage.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ScriptStats {
    /// Total number of scripts
    pub total_scripts: u64,
    /// Number of templates
    pub template_count: u64,
    /// Number of favorites
    pub favorite_count: u64,
    /// Most used script
    pub most_used: Option<Uuid>,
    /// Total executions
    pub total_executions: u64,
}

/// Import/Export format for scripts.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScriptExport {
    /// Format version
    pub version: u32,
    /// Export timestamp
    pub exported_at: DateTime<Utc>,
    /// List of exported scripts
    pub scripts: Vec<CommandScript>,
}

impl ScriptExport {
    /// Create a new export with the given scripts
    pub fn new(scripts: Vec<CommandScript>) -> Self {
        Self {
            version: 1,
            exported_at: Utc::now(),
            scripts,
        }
    }

    /// Serialize to JSON string
    pub fn to_json(&self) -> Result<String, serde_json::Error> {
        serde_json::to_string_pretty(self)
    }

    /// Deserialize from JSON string
    pub fn from_json(json: &str) -> Result<Self, serde_json::Error> {
        serde_json::from_str(json)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_script_type_display() {
        assert_eq!(ScriptType::Shell.to_string(), "shell");
        assert_eq!(ScriptType::Powershell.to_string(), "powershell");
        assert_eq!(ScriptType::Python.to_string(), "python");
    }

    #[test]
    fn test_script_type_from_str() {
        assert_eq!(ScriptType::from_str("shell"), Some(ScriptType::Shell));
        assert_eq!(ScriptType::from_str("SHELL"), Some(ScriptType::Shell));
        assert_eq!(ScriptType::from_str("bash"), Some(ScriptType::Shell));
        assert_eq!(ScriptType::from_str("pwsh"), Some(ScriptType::Powershell));
        assert_eq!(ScriptType::from_str("unknown"), None);
    }

    #[test]
    fn test_script_creation() {
        let script = CommandScript::new("Test Script", "echo hello");
        assert_eq!(script.name, "Test Script");
        assert_eq!(script.content, "echo hello");
        assert!(!script.is_template);
        assert!(!script.is_favorite);
        assert_eq!(script.version, 1);
        assert_eq!(script.usage_count, 0);
    }

    #[test]
    fn test_script_builder() {
        let script = CommandScript::new("My Script", "echo {{name}}")
            .with_type(ScriptType::Shell)
            .with_description("A test script")
            .with_tag("test")
            .with_tag("demo")
            .with_parameter(ScriptParameter::required_string("name"))
            .created_by("admin");

        assert_eq!(script.script_type, ScriptType::Shell);
        assert_eq!(script.description, Some("A test script".to_string()));
        assert_eq!(script.tags, vec!["test", "demo"]);
        assert_eq!(script.parameters.len(), 1);
        assert_eq!(script.created_by, Some("admin".to_string()));
    }

    #[test]
    fn test_script_execution_result() {
        let result = ScriptExecutionResult::new(Uuid::new_v4())
            .success(0, "hello", "", 100);

        assert!(result.success);
        assert_eq!(result.exit_code, Some(0));
        assert_eq!(result.stdout, "hello");
        assert_eq!(result.duration_ms, 100);
    }

    #[test]
    fn test_script_export_import() {
        let script = CommandScript::new("Test", "echo 1").with_type(ScriptType::Shell);
        let export = ScriptExport::new(vec![script.clone()]);
        let json = export.to_json().unwrap();
        let imported = ScriptExport::from_json(&json).unwrap();
        assert_eq!(imported.scripts.len(), 1);
        assert_eq!(imported.scripts[0].name, "Test");
    }

    #[test]
    fn test_parameter_types() {
        let string_param = ScriptParameter::required_string("host");
        assert_eq!(string_param.param_type, ParameterType::String);
        assert!(string_param.required);

        let select_param = ScriptParameter::select(
            "env",
            vec!["dev".to_string(), "prod".to_string()],
            Some("dev".to_string()),
        );
        assert_eq!(select_param.param_type, ParameterType::Select);
        assert_eq!(select_param.options, vec!["dev", "prod"]);
    }
}
