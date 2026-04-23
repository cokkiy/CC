//! Script Engine - Parsing and Parameter Substitution
//!
//! This module handles script parsing, parameter placeholder resolution,
//! and parameter substitution for command scripts.

use crate::scripts::{CommandScript, ParameterType, ScriptExecutionResult, ScriptParameter, ScriptType};
use chrono::Utc;
use regex::Regex;
use std::collections::HashMap;
use std::sync::OnceLock;
use uuid::Uuid;

/// Regular expression for matching parameter placeholders {{param_name}}
static PARAM_REGEX: OnceLock<Regex> = OnceLock::new();

fn get_param_regex() -> &'static Regex {
    PARAM_REGEX.get_or_init(|| Regex::new(r"\{\{(\w+)\}\}").unwrap())
}

/// Regular expression for matching environment variable placeholders ${VAR_NAME}
static ENV_REGEX: OnceLock<Regex> = OnceLock::new();

fn get_env_regex() -> &'static Regex {
    ENV_REGEX.get_or_init(|| Regex::new(r#"\$([A-Za-z_][A-Za-z0-9_]*)|\$\{([^}]+)\}"#).unwrap())
}

/// Errors that can occur during script processing.
#[derive(Debug, Clone, thiserror::Error)]
pub enum ScriptEngineError {
    /// Missing required parameter
    #[error("missing required parameter: {0}")]
    MissingParameter(String),

    /// Parameter validation failed
    #[error("parameter validation failed for '{name}': {reason}")]
    ValidationFailed { name: String, reason: String },

    /// Invalid parameter value
    #[error("invalid value for parameter '{name}': {value}")]
    InvalidValue { name: String, value: String },

    /// Script execution error
    #[error("script execution error: {0}")]
    ExecutionError(String),

    /// Unsupported script type
    #[error("unsupported script type: {0}")]
    UnsupportedScriptType(String),

    /// Template error
    #[error("template error: {0}")]
    TemplateError(String),
}

pub type ScriptEngineResult<T> = Result<T, ScriptEngineError>;

/// A resolved parameter value with validation info.
#[derive(Debug, Clone)]
pub struct ResolvedParameter {
    /// Parameter definition
    pub definition: ScriptParameter,
    /// Resolved value
    pub value: String,
}

/// Script engine for parsing and substituting parameters in scripts.
#[derive(Debug, Clone, Default)]
pub struct ScriptEngine {
    /// Collected parameters from the script
    detected_parameters: Vec<String>,
}

impl ScriptEngine {
    /// Create a new script engine instance
    pub fn new() -> Self {
        Self {
            detected_parameters: Vec::new(),
        }
    }

    /// Detect all parameter placeholders in a script's content
    ///
    /// Returns a list of unique parameter names found in {{param_name}} format.
    pub fn detect_parameters(content: &str) -> Vec<String> {
        let regex = get_param_regex();
        let mut params: Vec<String> = regex
            .captures_iter(content)
            .filter_map(|cap| cap.get(1).map(|m| m.as_str().to_string()))
            .collect();
        params.sort();
        params.dedup();
        params
    }

    /// Validate a single parameter value against its definition
    pub fn validate_parameter(param: &ScriptParameter, value: &str) -> ScriptEngineResult<()> {
        // Check required
        if param.required && (value.is_empty() || value.trim().is_empty()) {
            return Err(ScriptEngineError::MissingParameter(param.name.clone()));
        }

        // Check validation regex if provided
        if let Some(ref pattern) = param.validation {
            let regex = Regex::new(pattern);
            match regex {
                Ok(re) => {
                    if !re.is_match(value) {
                        return Err(ScriptEngineError::ValidationFailed {
                            name: param.name.clone(),
                            reason: format!(
                                "value '{}' does not match pattern '{}'",
                                value, pattern
                            ),
                        });
                    }
                }
                Err(e) => {
                    return Err(ScriptEngineError::ValidationFailed {
                        name: param.name.clone(),
                        reason: format!("invalid regex pattern: {}", e),
                    });
                }
            }
        }

        // Type-specific validation
        match param.param_type {
            ParameterType::Number => {
                if !value.is_empty() && value.parse::<f64>().is_err() {
                    return Err(ScriptEngineError::InvalidValue {
                        name: param.name.clone(),
                        value: value.to_string(),
                    });
                }
            }
            ParameterType::Boolean => {
                let lower = value.to_lowercase();
                if !lower.is_empty()
                    && !["true", "false", "1", "0", "yes", "no"].contains(&lower.as_str())
                {
                    return Err(ScriptEngineError::InvalidValue {
                        name: param.name.clone(),
                        value: value.to_string(),
                    });
                }
            }
            ParameterType::Select => {
                if !value.is_empty() && !param.options.contains(&value.to_string()) {
                    return Err(ScriptEngineError::InvalidValue {
                        name: param.name.clone(),
                        value: value.to_string(),
                    });
                }
            }
            ParameterType::String => {
                // No additional validation for strings
            }
        }

        Ok(())
    }

    /// Validate all parameters against their definitions
    pub fn validate_parameters(
        definitions: &[ScriptParameter],
        values: &HashMap<String, String>,
    ) -> ScriptEngineResult<()> {
        for param in definitions {
            let value = values.get(&param.name).map(|s| s.as_str()).unwrap_or("");
            Self::validate_parameter(param, value)?;
        }
        Ok(())
    }

    /// Resolve all parameters with their values, using defaults where not provided
    pub fn resolve_parameters(
        definitions: &[ScriptParameter],
        provided: &HashMap<String, String>,
    ) -> ScriptEngineResult<Vec<ResolvedParameter>> {
        let mut resolved = Vec::new();

        for def in definitions {
            let value = provided
                .get(&def.name)
                .map(|s| s.as_str())
                .unwrap_or("");

            let value_to_use = if value.is_empty() {
                def.default_value
                    .as_deref()
                    .unwrap_or("")
            } else {
                value
            };

            // Validate the resolved value
            Self::validate_parameter(def, value_to_use)?;

            resolved.push(ResolvedParameter {
                definition: def.clone(),
                value: value_to_use.to_string(),
            });
        }

        // Check for missing required parameters
        for param in definitions {
            if param.required {
                let provided_value = provided.get(&param.name).map(|s| s.as_str()).unwrap_or("");
                let has_default = param.default_value.is_some();
                if provided_value.is_empty() && !has_default {
                    return Err(ScriptEngineError::MissingParameter(param.name.clone()));
                }
            }
        }

        Ok(resolved)
    }

    /// Substitute parameters in script content
    ///
    /// Replaces all {{param_name}} placeholders with their corresponding values.
    /// If a parameter is not found, it leaves the placeholder unchanged.
    pub fn substitute_parameters(
        content: &str,
        values: &HashMap<String, String>,
    ) -> String {
        let regex = get_param_regex();
        regex
            .replace_all(content, |caps: &regex::Captures| {
                let param_name = &caps[1];
                values.get(param_name).cloned().unwrap_or_else(|| caps[0].to_string())
            })
            .to_string()
    }

    /// Substitute parameters and also resolve environment variables
    ///
    /// Replaces {{param}} placeholders first, then ${VAR} environment variables.
    pub fn substitute_all(content: &str, params: &HashMap<String, String>) -> String {
        let content = Self::substitute_parameters(content, params);
        let regex = get_env_regex();
        regex
            .replace_all(&content, |caps: &regex::Captures| {
                // Group 1 is $VAR, group 2 is ${VAR}
                let var_name = caps.get(1)
                    .or_else(|| caps.get(2))
                    .map(|m| m.as_str())
                    .unwrap_or(&caps[0]);
                std::env::var(var_name).unwrap_or_else(|_| caps[0].to_string())
            })
            .to_string()
    }

    /// Analyze a script to detect parameters and validate definitions
    ///
    /// Returns information about detected parameters, defined parameters,
    /// and any validation issues. Note: A script with required parameters
    /// that have no defaults is NOT considered invalid - those parameters
    /// just need to be provided at execution time.
    pub fn analyze_script(script: &CommandScript) -> ScriptEngineResult<ScriptAnalysis> {
        // Detect parameters in content
        let detected = Self::detect_parameters(&script.content);

        // Get defined parameter names
        let defined: Vec<&str> = script.parameters.iter().map(|p| p.name.as_str()).collect();

        // Find missing definitions (detected but not defined)
        let missing: Vec<String> = detected
            .iter()
            .filter(|d| !defined.contains(&d.as_str()))
            .cloned()
            .collect();

        // Find unused definitions (defined but not used in content)
        let unused: Vec<String> = defined
            .iter()
            .filter(|d| !detected.contains(&d.to_string()))
            .map(|s| s.to_string())
            .collect();

        // Validate parameter definitions only (not whether values are provided)
        // A required parameter without a default is NOT an error - it just needs to be provided at runtime
        let validation_errors: Vec<(String, ScriptEngineError)> = script
            .parameters
            .iter()
            .filter_map(|p| {
                // Only validate if there's a default value to validate
                if let Some(ref default) = p.default_value {
                    if let Err(e) = Self::validate_parameter(p, default) {
                        return Some((p.name.clone(), e));
                    }
                }
                None
            })
            .collect();

        // Script is valid if all detected parameters are defined
        // (having required params without defaults is fine - they just need values at runtime)
        let is_valid = missing.is_empty();

        Ok(ScriptAnalysis {
            detected_parameters: detected,
            defined_parameters: script.parameters.clone(),
            missing_definitions: missing.clone(),
            unused_definitions: unused,
            validation_errors: validation_errors.clone(),
            is_valid: missing.is_empty() && validation_errors.is_empty(),
        })
    }

    /// Render a script with all parameters resolved
    ///
    /// This is a convenience method that combines detection, validation,
    /// and substitution.
    pub fn render_script(
        script: &CommandScript,
        parameter_values: &HashMap<String, String>,
    ) -> ScriptEngineResult<RenderedScript> {
        // First analyze the script
        let analysis = Self::analyze_script(script)?;

        if !analysis.is_valid {
            // Return validation errors
            if !analysis.missing_definitions.is_empty() {
                return Err(ScriptEngineError::TemplateError(format!(
                    "missing parameter definitions: {:?}",
                    analysis.missing_definitions
                )));
            }
        }

        // Resolve parameters
        let resolved = Self::resolve_parameters(&script.parameters, parameter_values)?;

        // Build value map from resolved
        let resolved_values: HashMap<String, String> = resolved
            .iter()
            .map(|r| (r.definition.name.clone(), r.value.clone()))
            .collect();

        // Substitute in content
        let final_content = Self::substitute_parameters(&script.content, &resolved_values);

        Ok(RenderedScript {
            script_id: script.id,
            name: script.name.clone(),
            script_type: script.script_type,
            content: final_content,
            resolved_parameters: resolved,
        })
    }

    /// Build the shell command for executing a script
    ///
    /// Returns the command and arguments based on script type.
    pub fn build_execution_command(
        rendered: &RenderedScript,
    ) -> ScriptEngineResult<(String, Vec<String>)> {
        match rendered.script_type {
            ScriptType::Shell => Ok(("sh".to_string(), vec!["-c".to_string(), rendered.content.clone()])),
            ScriptType::Powershell => Ok((
                "pwsh".to_string(),
                vec!["-NoProfile".to_string(), "-Command".to_string(), rendered.content.clone()],
            )),
            ScriptType::Python => Ok((
                "python3".to_string(),
                vec!["-c".to_string(), rendered.content.clone()],
            )),
            ScriptType::Lua => Ok((
                "lua".to_string(),
                vec!["-e".to_string(), rendered.content.clone()],
            )),
        }
    }

    /// Check if content has any parameter placeholders
    pub fn has_parameters(content: &str) -> bool {
        get_param_regex().is_match(content)
    }

    /// Extract environment variable references from content
    pub fn detect_env_variables(content: &str) -> Vec<String> {
        let regex = get_env_regex();
        let mut vars: Vec<String> = regex
            .captures_iter(content)
            .filter_map(|cap| {
                cap.get(1)
                    .map(|m| m.as_str().to_string())
                    .or_else(|| cap.get(2).map(|m| m.as_str().to_string()))
            })
            .collect();
        vars.sort();
        vars.dedup();
        vars
    }
}

/// Analysis result from examining a script.
#[derive(Debug, Clone)]
pub struct ScriptAnalysis {
    /// Parameters detected in the content
    pub detected_parameters: Vec<String>,
    /// Parameter definitions from the script
    pub defined_parameters: Vec<ScriptParameter>,
    /// Parameters found in content but not defined
    pub missing_definitions: Vec<String>,
    /// Parameters defined but not used in content
    pub unused_definitions: Vec<String>,
    /// Validation errors
    pub validation_errors: Vec<(String, ScriptEngineError)>,
    /// Whether the script is valid
    pub is_valid: bool,
}

impl Default for ScriptAnalysis {
    fn default() -> Self {
        Self {
            detected_parameters: Vec::new(),
            defined_parameters: Vec::new(),
            missing_definitions: Vec::new(),
            unused_definitions: Vec::new(),
            validation_errors: Vec::new(),
            is_valid: true,
        }
    }
}

/// A rendered script ready for execution.
#[derive(Debug, Clone)]
pub struct RenderedScript {
    /// Original script ID
    pub script_id: Uuid,
    /// Script name
    pub name: String,
    /// Script type
    pub script_type: ScriptType,
    /// Final content with parameters substituted
    pub content: String,
    /// Resolved parameter values
    pub resolved_parameters: Vec<ResolvedParameter>,
}

/// Script execution context
#[derive(Debug, Clone, Default)]
pub struct ExecutionContext {
    /// Working directory
    pub working_dir: Option<String>,
    /// Environment variables to set/override
    pub env_vars: HashMap<String, String>,
    /// Timeout in seconds
    pub timeout_secs: u64,
    /// Whether to capture output
    pub capture_output: bool,
}

impl ExecutionContext {
    /// Create a new execution context with defaults
    pub fn new() -> Self {
        Self::default()
    }

    /// Set the working directory
    pub fn with_working_dir(mut self, dir: impl Into<String>) -> Self {
        self.working_dir = Some(dir.into());
        self
    }

    /// Add an environment variable
    pub fn with_env(mut self, key: impl Into<String>, value: impl Into<String>) -> Self {
        self.env_vars.insert(key.into(), value.into());
        self
    }

    /// Set the timeout in seconds
    pub fn with_timeout(mut self, secs: u64) -> Self {
        self.timeout_secs = secs;
        self
    }

    /// Enable output capture
    pub fn with_capture_output(mut self) -> Self {
        self.capture_output = true;
        self
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_detect_parameters() {
        let content = "echo {{greeting}} {{name}}";
        let params = ScriptEngine::detect_parameters(content);
        assert_eq!(params, vec!["greeting", "name"]);
    }

    #[test]
    fn test_detect_parameters_no_params() {
        let content = "echo hello world";
        let params = ScriptEngine::detect_parameters(content);
        assert!(params.is_empty());
    }

    #[test]
    fn test_detect_duplicate_params() {
        let content = "{{name}} {{name}} {{name}}";
        let params = ScriptEngine::detect_parameters(content);
        assert_eq!(params, vec!["name"]);
    }

    #[test]
    fn test_substitute_parameters() {
        let content = "echo {{greeting}}, {{name}}!";
        let mut values = HashMap::new();
        values.insert("greeting".to_string(), "Hello".to_string());
        values.insert("name".to_string(), "World".to_string());

        let result = ScriptEngine::substitute_parameters(content, &values);
        assert_eq!(result, "echo Hello, World!");
    }

    #[test]
    fn test_substitute_parameters_missing() {
        let content = "echo {{greeting}}, {{name}}!";
        let mut values = HashMap::new();
        values.insert("greeting".to_string(), "Hello".to_string());
        // 'name' is missing

        let result = ScriptEngine::substitute_parameters(content, &values);
        assert_eq!(result, "echo Hello, {{name}}!");
    }

    #[test]
    fn test_validate_parameter_required() {
        let param = ScriptParameter::required_string("host");
        
        // Empty value should fail for required
        let result = ScriptEngine::validate_parameter(&param, "");
        assert!(result.is_err());
    }

    #[test]
    fn test_validate_parameter_number() {
        let param = ScriptParameter {
            name: "port".to_string(),
            param_type: ParameterType::Number,
            ..Default::default()
        };

        assert!(ScriptEngine::validate_parameter(&param, "8080").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "abc").is_err());
    }

    #[test]
    fn test_validate_parameter_boolean() {
        let param = ScriptParameter {
            name: "verbose".to_string(),
            param_type: ParameterType::Boolean,
            ..Default::default()
        };

        assert!(ScriptEngine::validate_parameter(&param, "true").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "false").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "1").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "yes").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "maybe").is_err());
    }

    #[test]
    fn test_validate_parameter_select() {
        let param = ScriptParameter::select("env", vec!["dev".to_string(), "prod".to_string()], None);

        assert!(ScriptEngine::validate_parameter(&param, "dev").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "prod").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "staging").is_err());
    }

    #[test]
    fn test_validate_parameter_regex() {
        let mut param = ScriptParameter::required_string("ip");
        param.validation = Some(r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$".to_string());

        assert!(ScriptEngine::validate_parameter(&param, "192.168.1.1").is_ok());
        assert!(ScriptEngine::validate_parameter(&param, "invalid").is_err());
    }

    #[test]
    fn test_resolve_parameters_with_defaults() {
        let param = ScriptParameter::optional_string("port", "8080");
        let values = HashMap::new();

        let resolved = ScriptEngine::resolve_parameters(&[param], &values).unwrap();
        assert_eq!(resolved[0].value, "8080");
    }

    #[test]
    fn test_resolve_parameters_override_default() {
        let param = ScriptParameter::optional_string("port", "8080");
        let mut values = HashMap::new();
        values.insert("port".to_string(), "9090".to_string());

        let resolved = ScriptEngine::resolve_parameters(&[param], &values).unwrap();
        assert_eq!(resolved[0].value, "9090");
    }

    #[test]
    fn test_analyze_script() {
        let script = CommandScript::new("test", "echo {{name}}")
            .with_parameter(ScriptParameter::required_string("name"));

        let analysis = ScriptEngine::analyze_script(&script).unwrap();
        assert!(analysis.is_valid);
        assert_eq!(analysis.detected_parameters, vec!["name"]);
        assert!(analysis.missing_definitions.is_empty());
    }

    #[test]
    fn test_analyze_script_missing_definition() {
        let script = CommandScript::new("test", "echo {{name}} {{unknown}}");

        let analysis = ScriptEngine::analyze_script(&script).unwrap();
        assert!(!analysis.is_valid);
        assert!(analysis.missing_definitions.contains(&"unknown".to_string()));
    }

    #[test]
    fn test_render_script() {
        let script = CommandScript::new("test", "echo {{greeting}}, {{name}}!")
            .with_parameter(ScriptParameter::optional_string("greeting", "Hello"))
            .with_parameter(ScriptParameter::required_string("name"));

        let mut values = HashMap::new();
        values.insert("name".to_string(), "World".to_string());

        let rendered = ScriptEngine::render_script(&script, &values).unwrap();
        assert_eq!(rendered.content, "echo Hello, World!");
    }

    #[test]
    fn test_detect_env_variables() {
        // Test with a pattern that would match env vars if they existed
        // The function should detect the ${VAR} pattern regardless of whether the var exists
        let content = "export PATH=$PATH:${HOME} and ${USER}";
        let vars = ScriptEngine::detect_env_variables(content);
        assert!(vars.contains(&"PATH".to_string()));
        assert!(vars.contains(&"HOME".to_string()));
        assert!(vars.contains(&"USER".to_string()));
    }

    #[test]
    fn test_has_parameters() {
        assert!(ScriptEngine::has_parameters("echo {{name}}"));
        assert!(!ScriptEngine::has_parameters("echo hello"));
    }

    #[test]
    fn test_build_execution_command_shell() {
        let rendered = RenderedScript {
            script_id: Uuid::new_v4(),
            name: "test".to_string(),
            script_type: ScriptType::Shell,
            content: "echo hello".to_string(),
            resolved_parameters: Vec::new(),
        };

        let (cmd, args) = ScriptEngine::build_execution_command(&rendered).unwrap();
        assert_eq!(cmd, "sh");
        assert_eq!(args, vec!["-c", "echo hello"]);
    }

    #[test]
    fn test_build_execution_command_powershell() {
        let rendered = RenderedScript {
            script_id: Uuid::new_v4(),
            name: "test".to_string(),
            script_type: ScriptType::Powershell,
            content: "Write-Host 'hello'".to_string(),
            resolved_parameters: Vec::new(),
        };

        let (cmd, args) = ScriptEngine::build_execution_command(&rendered).unwrap();
        assert_eq!(cmd, "pwsh");
        assert!(args.contains(&"-NoProfile".to_string()));
        assert!(args.contains(&"-Command".to_string()));
    }

    #[test]
    fn test_substitute_all_with_env() {
        // SAFETY: This is a test-only operation to set an environment variable
        unsafe {
            std::env::set_var("TEST_VAR", "test_value");
        }

        // Note: ${VAR} format is required for env var substitution
        let content = "echo {{param}} and ${TEST_VAR}";
        let mut values = HashMap::new();
        values.insert("param".to_string(), "hello".to_string());

        let result = ScriptEngine::substitute_all(content, &values);
        assert_eq!(result, "echo hello and test_value");

        // SAFETY: This is a test-only operation to remove an environment variable
        unsafe {
            std::env::remove_var("TEST_VAR");
        }
    }
}
