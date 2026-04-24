//! Station Groups Store - CRUD Operations for Station Groups
//!
//! This module provides persistent storage and CRUD operations for station groups.

use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::{Arc, RwLock};

use anyhow::{Context, Result};
use chrono::{DateTime, Utc};
use rusqlite::{params, Connection};
use uuid::Uuid;

use crate::groups::{StationGroup, StationGroupFilter, StationGroupSortField, StationGroupStats};

/// In-memory cache for station groups
pub struct StationGroupCache {
    groups: RwLock<HashMap<Uuid, StationGroup>>,
    stats: RwLock<StationGroupStats>,
}

impl StationGroupCache {
    pub fn new() -> Self {
        Self {
            groups: RwLock::new(HashMap::new()),
            stats: RwLock::new(StationGroupStats::default()),
        }
    }

    pub fn get(&self, id: &Uuid) -> Option<StationGroup> {
        self.groups.read().ok()?.get(id).cloned()
    }

    pub fn get_by_name(&self, name: &str) -> Option<StationGroup> {
        self.groups
            .read()
            .ok()?
            .values()
            .find(|g| g.name == name)
            .cloned()
    }

    pub fn get_all(&self) -> Vec<StationGroup> {
        self.groups
            .read()
            .map(|groups| groups.values().cloned().collect())
            .unwrap_or_default()
    }

    pub fn insert(&self, group: StationGroup) {
        let mut groups = self.groups.write().unwrap();
        groups.insert(group.id, group.clone());
        drop(groups);
        self.update_stats();
    }

    pub fn update(&self, group: &StationGroup) -> Option<StationGroup> {
        let mut groups = self.groups.write().unwrap();
        if groups.contains_key(&group.id) {
            let old = groups.get(&group.id).cloned();
            groups.insert(group.id, group.clone());
            drop(groups);
            self.update_stats();
            old
        } else {
            None
        }
    }

    pub fn remove(&self, id: &Uuid) -> Option<StationGroup> {
        let mut groups = self.groups.write().unwrap();
        let removed = groups.remove(id);
        drop(groups);
        if removed.is_some() {
            self.update_stats();
        }
        removed
    }

    pub fn filter(&self, filter: &StationGroupFilter) -> Vec<StationGroup> {
        let groups: Vec<StationGroup> = self.get_all();

        let mut filtered: Vec<StationGroup> = groups
            .into_iter()
            .filter(|group| {
                if let Some(name) = &filter.name {
                    if !group.name.to_lowercase().contains(&name.to_lowercase()) {
                        return false;
                    }
                }
                if let Some(created_by) = &filter.created_by {
                    if group.created_by.as_deref() != Some(created_by.as_str()) {
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
                StationGroupSortField::CreatedAt => a.created_at.cmp(&b.created_at),
                StationGroupSortField::UpdatedAt => a.updated_at.cmp(&b.updated_at),
                StationGroupSortField::Name => a.name.cmp(&b.name),
                StationGroupSortField::StationCount => a.station_ids.len().cmp(&b.station_ids.len()),
            };
            if sort_desc {
                cmp.reverse()
            } else {
                cmp
            }
        });

        // Pagination (limit 0 means no limit)
        let offset = filter.offset as usize;
        let limit = if filter.limit == 0 {
            usize::MAX
        } else {
            filter.limit as usize
        };
        filtered.into_iter().skip(offset).take(limit).collect()
    }

    fn update_stats(&self) {
        let groups = self.groups.read().unwrap();
        let mut stats = self.stats.write().unwrap();

        stats.total_groups = groups.len() as u64;
        let total_stations: usize = groups.values().map(|g| g.station_ids.len()).sum();
        stats.total_stations = total_stations as u64;
        stats.avg_stations_per_group = if groups.is_empty() {
            0.0
        } else {
            total_stations as f64 / groups.len() as f64
        };
    }

    pub fn stats(&self) -> StationGroupStats {
        self.stats.read().unwrap().clone()
    }
}

impl Default for StationGroupCache {
    fn default() -> Self {
        Self::new()
    }
}

/// Station group store with SQLite persistence.
pub struct StationGroupStore {
    cache: Arc<StationGroupCache>,
    db_path: PathBuf,
}

impl StationGroupStore {
    /// Create a new station group store with the given database path.
    pub fn new(db_path: PathBuf) -> Result<Self> {
        let store = Self {
            cache: Arc::new(StationGroupCache::new()),
            db_path,
        };
        store.init_db()?;
        store.load_from_db()?;
        Ok(store)
    }

    /// Initialize the database schema.
    fn init_db(&self) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open station group store DB at {}", self.db_path.display()))?;

        conn.execute(
            "CREATE TABLE IF NOT EXISTS station_groups (
                id TEXT PRIMARY KEY,
                name TEXT NOT NULL UNIQUE,
                description TEXT,
                station_ids TEXT NOT NULL,
                created_at TEXT NOT NULL,
                updated_at TEXT NOT NULL,
                created_by TEXT
            )",
            [],
        )?;

        conn.execute(
            "CREATE INDEX IF NOT EXISTS idx_station_groups_name ON station_groups(name)",
            [],
        )?;

        Ok(())
    }

    /// Load groups from the database into cache.
    fn load_from_db(&self) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open station group store DB at {}", self.db_path.display()))?;

        let mut stmt = conn.prepare(
            "SELECT id, name, description, station_ids, created_at, updated_at, created_by
             FROM station_groups ORDER BY created_at DESC",
        )?;

        let row_data_list: Vec<GroupRowData> = stmt
            .query_map([], |row| {
                let created_at_str: String = row.get(4)?;
                let updated_at_str: String = row.get(5)?;

                Ok(GroupRowData {
                    id: row.get(0)?,
                    name: row.get(1)?,
                    description: row.get(2)?,
                    station_ids: row.get(3)?,
                    created_at: created_at_str,
                    updated_at: updated_at_str,
                    created_by: row.get(6)?,
                })
            })?
            .filter_map(|r| r.ok())
            .collect();

        for data in row_data_list {
            if let Ok(group) = self.row_data_to_group(data) {
                self.cache.insert(group);
            }
        }

        Ok(())
    }

    fn row_data_to_group(&self, data: GroupRowData) -> Result<StationGroup> {
        let station_ids: Vec<String> =
            serde_json::from_str(&data.station_ids).unwrap_or_default();

        let created_at = DateTime::parse_from_rfc3339(&data.created_at)
            .map(|dt| dt.with_timezone(&Utc))
            .unwrap_or_else(|_| Utc::now());
        let updated_at = DateTime::parse_from_rfc3339(&data.updated_at)
            .map(|dt| dt.with_timezone(&Utc))
            .unwrap_or_else(|_| Utc::now());

        Ok(StationGroup {
            id: Uuid::parse_str(&data.id).unwrap_or_else(|_| Uuid::new_v4()),
            name: data.name,
            description: data.description,
            station_ids,
            created_at,
            updated_at,
            created_by: data.created_by,
        })
    }

    // CRUD Operations

    /// Create a new station group.
    pub fn create(&self, group: &StationGroup) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open station group store DB at {}", self.db_path.display()))?;

        let station_ids_json = serde_json::to_string(&group.station_ids)?;

        conn.execute(
            "INSERT INTO station_groups (id, name, description, station_ids, created_at, updated_at, created_by)
             VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)",
            params![
                group.id.to_string(),
                group.name,
                group.description,
                station_ids_json,
                group.created_at.to_rfc3339(),
                group.updated_at.to_rfc3339(),
                group.created_by,
            ],
        )?;

        self.cache.insert(group.clone());
        Ok(())
    }

    /// Get a station group by ID.
    pub fn get(&self, id: &Uuid) -> Option<StationGroup> {
        self.cache.get(id)
    }

    /// Get a station group by name.
    pub fn get_by_name(&self, name: &str) -> Option<StationGroup> {
        self.cache.get_by_name(name)
    }

    /// Get all station groups.
    pub fn get_all(&self) -> Vec<StationGroup> {
        self.cache.get_all()
    }

    /// Update a station group.
    pub fn update(&self, group: &StationGroup) -> Result<Option<StationGroup>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open station group store DB at {}", self.db_path.display()))?;

        let station_ids_json = serde_json::to_string(&group.station_ids)?;

        let affected = conn.execute(
            "UPDATE station_groups SET name = ?2, description = ?3, station_ids = ?4,
                                    updated_at = ?5, created_by = ?6
             WHERE id = ?1",
            params![
                group.id.to_string(),
                group.name,
                group.description,
                station_ids_json,
                group.updated_at.to_rfc3339(),
                group.created_by,
            ],
        )?;

        if affected > 0 {
            self.cache.update(group);
            Ok(self.cache.get(&group.id))
        } else {
            Ok(None)
        }
    }

    /// Delete a station group by ID.
    pub fn delete(&self, id: &Uuid) -> Result<Option<StationGroup>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open station group store DB at {}", self.db_path.display()))?;

        let affected = conn.execute(
            "DELETE FROM station_groups WHERE id = ?1",
            [id.to_string()],
        )?;

        if affected > 0 {
            Ok(self.cache.remove(id))
        } else {
            Ok(None)
        }
    }

    /// Filter station groups by criteria.
    pub fn filter(&self, filter: &StationGroupFilter) -> Vec<StationGroup> {
        self.cache.filter(filter)
    }

    /// Get station group statistics.
    pub fn stats(&self) -> StationGroupStats {
        self.cache.stats()
    }
}

#[derive(Debug)]
struct GroupRowData {
    id: String,
    name: String,
    description: Option<String>,
    station_ids: String,
    created_at: String,
    updated_at: String,
    created_by: Option<String>,
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::env::temp_dir;

    #[test]
    fn test_station_group_crud() {
        let db_path = temp_dir().join("station_group_test.db");
        // Clean up any stale data from previous test runs
        std::fs::remove_file(db_path.clone()).ok();
        let store = StationGroupStore::new(db_path.clone()).unwrap();

        let mut group = StationGroup::new("Test Group");
        group = group.with_description("A test station group");
        group = group.created_by("test-user");
        group = group.add_station("station-1").add_station("station-2");

        // Create
        store.create(&group).unwrap();
        assert!(store.get(&group.id).is_some());

        // Read
        let retrieved = store.get(&group.id).unwrap();
        assert_eq!(retrieved.name, "Test Group");
        assert_eq!(retrieved.description, Some("A test station group".to_string()));
        assert_eq!(retrieved.station_ids.len(), 2);

        // Update
        let mut updated = retrieved;
        updated = updated.add_station("station-3");
        store.update(&updated).unwrap();

        let retrieved2 = store.get(&group.id).unwrap();
        assert_eq!(retrieved2.station_ids.len(), 3);

        // Delete
        store.delete(&group.id).unwrap();
        assert!(store.get(&group.id).is_none());

        // Clean up
        std::fs::remove_file(db_path).ok();
    }

    #[test]
    fn test_station_group_filter() {
        let db_path = temp_dir().join("station_group_filter_test.db");
        // Clean up any stale data from previous test runs
        std::fs::remove_file(db_path.clone()).ok();
        let store = StationGroupStore::new(db_path.clone()).unwrap();

        // Create groups with different names
        for i in 0..5 {
            let mut group = StationGroup::new(format!("Group {}", i));
            group = group.add_stations(["station-a", "station-b"]);
            store.create(&group).unwrap();
        }

        // Verify groups are in cache
        let all_groups = store.get_all();
        assert_eq!(all_groups.len(), 5, "Should have 5 groups in cache");

        // Filter by name
        let filter = StationGroupFilter {
            name: Some("Group 0".to_string()),
            ..Default::default()
        };
        let filtered = store.filter(&filter);
        assert_eq!(filtered.len(), 1, "Should have 1 matching group");

        // Filter by name partial match
        let filter = StationGroupFilter {
            name: Some("Group".to_string()),
            ..Default::default()
        };
        let filtered = store.filter(&filter);
        assert_eq!(filtered.len(), 5, "Should have 5 matching groups");

        // Clean up
        std::fs::remove_file(db_path).ok();
    }

    #[test]
    fn test_station_group_stats() {
        let db_path = temp_dir().join("station_group_stats_test.db");
        let store = StationGroupStore::new(db_path.clone()).unwrap();

        // Create groups with stations
        for i in 0..3 {
            let mut group = StationGroup::new(format!("Group {}", i));
            group = group.add_stations([
                format!("station-{}", i),
                format!("station-{}", i + 10),
            ]);
            store.create(&group).unwrap();
        }

        let stats = store.stats();
        assert_eq!(stats.total_groups, 3);
        assert_eq!(stats.total_stations, 6);
        assert_eq!(stats.avg_stations_per_group, 2.0);

        // Clean up
        std::fs::remove_file(db_path).ok();
    }
}