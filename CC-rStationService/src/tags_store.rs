//! Tags Store - CRUD Operations for Tags and Station Tags
//!
//! This module provides persistent storage and CRUD operations for tags
//! and station-tag assignments.

use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::{Arc, RwLock};

use anyhow::{Context, Result};
use chrono::{DateTime, Utc};
use rusqlite::{params, Connection};
use uuid::Uuid;

use crate::tags::{StationTag, StationTagFilter, StationTagSortField, Tag, TagFilter, TagSortField, TagStats};

/// In-memory cache for tags
pub struct TagCache {
    tags: RwLock<HashMap<Uuid, Tag>>,
    stats: RwLock<TagStats>,
}

impl TagCache {
    pub fn new() -> Self {
        Self {
            tags: RwLock::new(HashMap::new()),
            stats: RwLock::new(TagStats::default()),
        }
    }

    pub fn get(&self, id: &Uuid) -> Option<Tag> {
        self.tags.read().ok()?.get(id).cloned()
    }

    pub fn get_by_name(&self, name: &str) -> Option<Tag> {
        self.tags
            .read()
            .ok()?
            .values()
            .find(|t| t.name == name)
            .cloned()
    }

    pub fn get_all(&self) -> Vec<Tag> {
        self.tags
            .read()
            .map(|tags| tags.values().cloned().collect())
            .unwrap_or_default()
    }

    pub fn insert(&self, tag: Tag) {
        let mut tags = self.tags.write().unwrap();
        tags.insert(tag.id, tag.clone());
        drop(tags);
        self.update_stats();
    }

    pub fn update(&self, tag: &Tag) -> Option<Tag> {
        let mut tags = self.tags.write().unwrap();
        if tags.contains_key(&tag.id) {
            let old = tags.get(&tag.id).cloned();
            tags.insert(tag.id, tag.clone());
            drop(tags);
            self.update_stats();
            old
        } else {
            None
        }
    }

    pub fn remove(&self, id: &Uuid) -> Option<Tag> {
        let mut tags = self.tags.write().unwrap();
        let removed = tags.remove(id);
        drop(tags);
        if removed.is_some() {
            self.update_stats();
        }
        removed
    }

    pub fn filter(&self, filter: &TagFilter) -> Vec<Tag> {
        let tags: Vec<Tag> = self.get_all();

        let mut filtered: Vec<Tag> = tags
            .into_iter()
            .filter(|tag| {
                if let Some(name) = &filter.name {
                    if !tag.name.to_lowercase().contains(&name.to_lowercase()) {
                        return false;
                    }
                }
                if let Some(created_by) = &filter.created_by {
                    if tag.created_by.as_deref() != Some(created_by.as_str()) {
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
                TagSortField::CreatedAt => a.created_at.cmp(&b.created_at),
                TagSortField::UpdatedAt => a.updated_at.cmp(&b.updated_at),
                TagSortField::Name => a.name.cmp(&b.name),
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
        let tags = self.tags.read().unwrap();
        let mut stats = self.stats.write().unwrap();
        stats.total_tags = tags.len() as u64;
    }

    pub fn stats(&self) -> TagStats {
        self.stats.read().unwrap().clone()
    }
}

impl Default for TagCache {
    fn default() -> Self {
        Self::new()
    }
}

/// In-memory cache for station tags
pub struct StationTagCache {
    station_tags: RwLock<HashMap<String, Vec<StationTag>>>, // station_id -> tags
    tag_stations: RwLock<HashMap<Uuid, Vec<StationTag>>>,     // tag_id -> stations
}

impl StationTagCache {
    pub fn new() -> Self {
        Self {
            station_tags: RwLock::new(HashMap::new()),
            tag_stations: RwLock::new(HashMap::new()),
        }
    }

    pub fn get_by_station(&self, station_id: &str) -> Vec<StationTag> {
        self.station_tags
            .read()
            .ok()
            .and_then(|tags| tags.get(station_id).cloned())
            .unwrap_or_default()
    }

    pub fn get_by_tag(&self, tag_id: &Uuid) -> Vec<StationTag> {
        self.tag_stations
            .read()
            .ok()
            .and_then(|tags| tags.get(tag_id).cloned())
            .unwrap_or_default()
    }

    pub fn get_all(&self) -> Vec<StationTag> {
        self.station_tags
            .read()
            .map(|tags| tags.values().flatten().cloned().collect())
            .unwrap_or_default()
    }

    pub fn insert(&self, station_tag: StationTag) {
        let station_id = station_tag.station_id.clone();
        let tag_id = station_tag.tag_id;

        // Add to station_tags
        let mut station_tags = self.station_tags.write().unwrap();
        station_tags
            .entry(station_id.clone())
            .or_insert_with(Vec::new)
            .push(station_tag.clone());
        drop(station_tags);

        // Add to tag_stations
        let mut tag_stations = self.tag_stations.write().unwrap();
        tag_stations
            .entry(tag_id)
            .or_insert_with(Vec::new)
            .push(station_tag);
    }

    pub fn remove(&self, station_id: &str, tag_id: &Uuid) -> Option<StationTag> {
        // Remove from station_tags
        let mut station_tags = self.station_tags.write().unwrap();
        let removed = station_tags
            .get_mut(station_id)
            .and_then(|tags| {
                tags.iter()
                    .position(|t| t.tag_id == *tag_id)
                    .map(|idx| tags.remove(idx))
            });
        drop(station_tags);

        // Remove from tag_stations
        if let Some(removed_tag) = &removed {
            let mut tag_stations = self.tag_stations.write().unwrap();
            if let Some(stations) = tag_stations.get_mut(&removed_tag.tag_id) {
                stations.retain(|t| t.station_id != station_id || t.tag_id != *tag_id);
            }
        }

        removed
    }

    pub fn remove_by_station(&self, station_id: &str) -> Vec<StationTag> {
        let mut station_tags = self.station_tags.write().unwrap();
        let removed = station_tags.remove(station_id).unwrap_or_default();
        drop(station_tags);

        // Remove from tag_stations
        let mut tag_stations = self.tag_stations.write().unwrap();
        for tag in &removed {
            if let Some(stations) = tag_stations.get_mut(&tag.tag_id) {
                stations.retain(|t| t.station_id != station_id);
            }
        }

        removed
    }

    pub fn filter(&self, filter: &StationTagFilter) -> Vec<StationTag> {
        let all_tags: Vec<StationTag> = self.get_all();

        let mut filtered: Vec<StationTag> = all_tags
            .into_iter()
            .filter(|st| {
                if let Some(station_id) = &filter.station_id {
                    if &st.station_id != station_id {
                        return false;
                    }
                }
                if let Some(tag_id) = &filter.tag_id {
                    if st.tag_id != *tag_id {
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
                StationTagSortField::AssignedAt => a.assigned_at.cmp(&b.assigned_at),
                StationTagSortField::StationId => a.station_id.cmp(&b.station_id),
                StationTagSortField::TagId => a.tag_id.cmp(&b.tag_id),
            };
            if sort_desc {
                cmp.reverse()
            } else {
                cmp
            }
        });

        // Pagination
        let offset = filter.offset as usize;
        let limit = if filter.limit == 0 {
            usize::MAX
        } else {
            filter.limit as usize
        };
        filtered.into_iter().skip(offset).take(limit).collect()
    }

    pub fn total_assignments(&self) -> u64 {
        self.station_tags
            .read()
            .map(|tags| tags.values().map(|v| v.len()).sum::<usize>() as u64)
            .unwrap_or(0)
    }
}

impl Default for StationTagCache {
    fn default() -> Self {
        Self::new()
    }
}

/// Tag store with SQLite persistence.
pub struct TagStore {
    cache: Arc<TagCache>,
    station_tag_cache: Arc<StationTagCache>,
    db_path: PathBuf,
}

impl TagStore {
    /// Create a new tag store with the given database path.
    pub fn new(db_path: PathBuf) -> Result<Self> {
        let store = Self {
            cache: Arc::new(TagCache::new()),
            station_tag_cache: Arc::new(StationTagCache::new()),
            db_path,
        };
        store.init_db()?;
        store.load_from_db()?;
        Ok(store)
    }

    /// Initialize the database schema.
    fn init_db(&self) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        conn.execute(
            "CREATE TABLE IF NOT EXISTS tags (
                id TEXT PRIMARY KEY,
                name TEXT NOT NULL UNIQUE,
                description TEXT,
                color TEXT,
                created_at TEXT NOT NULL,
                updated_at TEXT NOT NULL,
                created_by TEXT
            )",
            [],
        )?;

        conn.execute(
            "CREATE INDEX IF NOT EXISTS idx_tags_name ON tags(name)",
            [],
        )?;

        conn.execute(
            "CREATE TABLE IF NOT EXISTS station_tags (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                station_id TEXT NOT NULL,
                tag_id TEXT NOT NULL,
                assigned_at TEXT NOT NULL,
                assigned_by TEXT,
                UNIQUE(station_id, tag_id)
            )",
            [],
        )?;

        conn.execute(
            "CREATE INDEX IF NOT EXISTS idx_station_tags_station_id ON station_tags(station_id)",
            [],
        )?;

        conn.execute(
            "CREATE INDEX IF NOT EXISTS idx_station_tags_tag_id ON station_tags(tag_id)",
            [],
        )?;

        Ok(())
    }

    /// Load tags and station tags from the database into cache.
    fn load_from_db(&self) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        // Load tags
        let mut stmt = conn.prepare(
            "SELECT id, name, description, color, created_at, updated_at, created_by
             FROM tags ORDER BY created_at DESC",
        )?;

        let tag_rows: Vec<TagRowData> = stmt
            .query_map([], |row| {
                let created_at_str: String = row.get(4)?;
                let updated_at_str: String = row.get(5)?;

                Ok(TagRowData {
                    id: row.get(0)?,
                    name: row.get(1)?,
                    description: row.get(2)?,
                    color: row.get(3)?,
                    created_at: created_at_str,
                    updated_at: updated_at_str,
                    created_by: row.get(6)?,
                })
            })?
            .filter_map(|r| r.ok())
            .collect();

        for data in tag_rows {
            if let Ok(tag) = self.row_data_to_tag(data) {
                self.cache.insert(tag);
            }
        }

        // Load station tags
        let mut stmt = conn.prepare(
            "SELECT station_id, tag_id, assigned_at, assigned_by FROM station_tags",
        )?;

        let station_tag_rows: Vec<StationTagRowData> = stmt
            .query_map([], |row| {
                let assigned_at_str: String = row.get(2)?;
                Ok(StationTagRowData {
                    station_id: row.get(0)?,
                    tag_id: row.get(1)?,
                    assigned_at: assigned_at_str,
                    assigned_by: row.get(3)?,
                })
            })?
            .filter_map(|r| r.ok())
            .collect();

        for data in station_tag_rows {
            if let Ok(station_tag) = self.row_data_to_station_tag(data) {
                self.station_tag_cache.insert(station_tag);
            }
        }

        Ok(())
    }

    fn row_data_to_tag(&self, data: TagRowData) -> Result<Tag> {
        let created_at = DateTime::parse_from_rfc3339(&data.created_at)
            .map(|dt| dt.with_timezone(&Utc))
            .unwrap_or_else(|_| Utc::now());
        let updated_at = DateTime::parse_from_rfc3339(&data.updated_at)
            .map(|dt| dt.with_timezone(&Utc))
            .unwrap_or_else(|_| Utc::now());

        Ok(Tag {
            id: Uuid::parse_str(&data.id).unwrap_or_else(|_| Uuid::new_v4()),
            name: data.name,
            description: data.description,
            color: data.color,
            created_at,
            updated_at,
            created_by: data.created_by,
        })
    }

    fn row_data_to_station_tag(&self, data: StationTagRowData) -> Result<StationTag> {
        let assigned_at = DateTime::parse_from_rfc3339(&data.assigned_at)
            .map(|dt| dt.with_timezone(&Utc))
            .unwrap_or_else(|_| Utc::now());

        Ok(StationTag {
            station_id: data.station_id,
            tag_id: Uuid::parse_str(&data.tag_id).unwrap_or_else(|_| Uuid::new_v4()),
            assigned_at,
            assigned_by: data.assigned_by,
        })
    }

    // Tag CRUD Operations

    /// Create a new tag.
    pub fn create_tag(&self, tag: &Tag) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        conn.execute(
            "INSERT INTO tags (id, name, description, color, created_at, updated_at, created_by)
             VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)",
            params![
                tag.id.to_string(),
                tag.name,
                tag.description,
                tag.color,
                tag.created_at.to_rfc3339(),
                tag.updated_at.to_rfc3339(),
                tag.created_by,
            ],
        )?;

        self.cache.insert(tag.clone());
        Ok(())
    }

    /// Get a tag by ID.
    pub fn get_tag(&self, id: &Uuid) -> Option<Tag> {
        self.cache.get(id)
    }

    /// Get a tag by name.
    pub fn get_tag_by_name(&self, name: &str) -> Option<Tag> {
        self.cache.get_by_name(name)
    }

    /// Get all tags.
    pub fn get_all_tags(&self) -> Vec<Tag> {
        self.cache.get_all()
    }

    /// Update a tag.
    pub fn update_tag(&self, tag: &Tag) -> Result<Option<Tag>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        let affected = conn.execute(
            "UPDATE tags SET name = ?2, description = ?3, color = ?4,
                            updated_at = ?5, created_by = ?6
             WHERE id = ?1",
            params![
                tag.id.to_string(),
                tag.name,
                tag.description,
                tag.color,
                tag.updated_at.to_rfc3339(),
                tag.created_by,
            ],
        )?;

        if affected > 0 {
            self.cache.update(tag);
            Ok(self.cache.get(&tag.id))
        } else {
            Ok(None)
        }
    }

    /// Delete a tag by ID.
    pub fn delete_tag(&self, id: &Uuid) -> Result<Option<Tag>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        let affected = conn.execute(
            "DELETE FROM tags WHERE id = ?1",
            [id.to_string()],
        )?;

        if affected > 0 {
            // Also delete all station-tag assignments for this tag
            conn.execute(
                "DELETE FROM station_tags WHERE tag_id = ?1",
                [id.to_string()],
            )?;
            Ok(self.cache.remove(id))
        } else {
            Ok(None)
        }
    }

    /// Filter tags by criteria.
    pub fn filter_tags(&self, filter: &TagFilter) -> Vec<Tag> {
        self.cache.filter(filter)
    }

    // Station Tag CRUD Operations

    /// Assign a tag to a station.
    pub fn assign_tag(&self, station_tag: &StationTag) -> Result<()> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        conn.execute(
            "INSERT OR REPLACE INTO station_tags (station_id, tag_id, assigned_at, assigned_by)
             VALUES (?1, ?2, ?3, ?4)",
            params![
                station_tag.station_id,
                station_tag.tag_id.to_string(),
                station_tag.assigned_at.to_rfc3339(),
                station_tag.assigned_by,
            ],
        )?;

        self.station_tag_cache.insert(station_tag.clone());
        Ok(())
    }

    /// Get tags for a station.
    pub fn get_tags_for_station(&self, station_id: &str) -> Vec<StationTag> {
        self.station_tag_cache.get_by_station(station_id)
    }

    /// Get stations for a tag.
    pub fn get_stations_for_tag(&self, tag_id: &Uuid) -> Vec<StationTag> {
        self.station_tag_cache.get_by_tag(tag_id)
    }

    /// Get all station-tag assignments.
    pub fn get_all_station_tags(&self) -> Vec<StationTag> {
        self.station_tag_cache.get_all()
    }

    /// Remove a tag from a station.
    pub fn remove_tag_from_station(&self, station_id: &str, tag_id: &Uuid) -> Result<Option<StationTag>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        let affected = conn.execute(
            "DELETE FROM station_tags WHERE station_id = ?1 AND tag_id = ?2",
            params![station_id, tag_id.to_string()],
        )?;

        if affected > 0 {
            Ok(self.station_tag_cache.remove(station_id, tag_id))
        } else {
            Ok(None)
        }
    }

    /// Remove all tags from a station.
    pub fn remove_all_tags_from_station(&self, station_id: &str) -> Result<Vec<StationTag>> {
        let conn = Connection::open(&self.db_path)
            .with_context(|| format!("open tag store DB at {}", self.db_path.display()))?;

        conn.execute(
            "DELETE FROM station_tags WHERE station_id = ?1",
            [station_id],
        )?;

        Ok(self.station_tag_cache.remove_by_station(station_id))
    }

    /// Filter station tags by criteria.
    pub fn filter_station_tags(&self, filter: &StationTagFilter) -> Vec<StationTag> {
        self.station_tag_cache.filter(filter)
    }

    /// Get combined tag statistics.
    pub fn stats(&self) -> TagStats {
        let mut stats = self.cache.stats();
        stats.total_assignments = self.station_tag_cache.total_assignments();
        stats
    }
}

#[derive(Debug)]
struct TagRowData {
    id: String,
    name: String,
    description: Option<String>,
    color: Option<String>,
    created_at: String,
    updated_at: String,
    created_by: Option<String>,
}

#[derive(Debug)]
struct StationTagRowData {
    station_id: String,
    tag_id: String,
    assigned_at: String,
    assigned_by: Option<String>,
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::env::temp_dir;

    #[test]
    fn test_tag_crud() {
        let db_path = temp_dir().join("tag_test.db");
        // Clean up any stale data from previous test runs
        std::fs::remove_file(db_path.clone()).ok();
        let store = TagStore::new(db_path.clone()).unwrap();

        let mut tag = Tag::new("Test Tag");
        tag = tag.with_description("A test tag");
        tag = tag.with_color("#FF5733");
        tag = tag.created_by("test-user");

        // Create
        store.create_tag(&tag).unwrap();
        assert!(store.get_tag(&tag.id).is_some());

        // Read
        let retrieved = store.get_tag(&tag.id).unwrap();
        assert_eq!(retrieved.name, "Test Tag");
        assert_eq!(retrieved.description, Some("A test tag".to_string()));
        assert_eq!(retrieved.color, Some("#FF5733".to_string()));

        // Update
        let mut updated = retrieved;
        updated = updated.with_color("#00FF00");
        store.update_tag(&updated).unwrap();

        let retrieved2 = store.get_tag(&tag.id).unwrap();
        assert_eq!(retrieved2.color, Some("#00FF00".to_string()));

        // Delete
        store.delete_tag(&tag.id).unwrap();
        assert!(store.get_tag(&tag.id).is_none());

        // Clean up
        std::fs::remove_file(db_path).ok();
    }

    #[test]
    fn test_station_tag_crud() {
        let db_path = temp_dir().join("station_tag_test.db");
        // Clean up any stale data from previous test runs
        std::fs::remove_file(db_path.clone()).ok();
        let store = TagStore::new(db_path.clone()).unwrap();

        let tag = Tag::new("Production");
        store.create_tag(&tag).unwrap();

        // Assign tag to station
        let station_tag = StationTag::new("station-1", tag.id).assigned_by("admin");
        store.assign_tag(&station_tag).unwrap();

        // Get tags for station
        let tags = store.get_tags_for_station("station-1");
        assert_eq!(tags.len(), 1);
        assert_eq!(tags[0].tag_id, tag.id);

        // Get stations for tag
        let stations = store.get_stations_for_tag(&tag.id);
        assert_eq!(stations.len(), 1);
        assert_eq!(stations[0].station_id, "station-1");

        // Remove tag from station
        store.remove_tag_from_station("station-1", &tag.id).unwrap();
        let tags = store.get_tags_for_station("station-1");
        assert!(tags.is_empty());

        // Clean up
        std::fs::remove_file(db_path).ok();
    }

    #[test]
    fn test_tag_filter() {
        let db_path = temp_dir().join("tag_filter_test.db");
        // Clean up any stale data from previous test runs
        std::fs::remove_file(db_path.clone()).ok();
        let store = TagStore::new(db_path.clone()).unwrap();

        // Create tags with different names
        for i in 0..5 {
            let tag = Tag::new(format!("Tag {}", i));
            store.create_tag(&tag).unwrap();
        }

        // Verify tags are in cache
        let all_tags = store.get_all_tags();
        assert_eq!(all_tags.len(), 5, "Should have 5 tags in cache");

        // Filter by name
        let filter = TagFilter {
            name: Some("Tag 0".to_string()),
            ..Default::default()
        };
        let filtered = store.filter_tags(&filter);
        assert_eq!(filtered.len(), 1, "Should have 1 matching tag");

        // Filter by name partial match
        let filter = TagFilter {
            name: Some("Tag".to_string()),
            ..Default::default()
        };
        let filtered = store.filter_tags(&filter);
        assert_eq!(filtered.len(), 5, "Should have 5 matching tags");

        // Clean up
        std::fs::remove_file(db_path).ok();
    }

    #[test]
    fn test_tag_stats() {
        let db_path = temp_dir().join("tag_stats_test.db");
        let store = TagStore::new(db_path.clone()).unwrap();

        // Create tags
        for i in 0..3 {
            let tag = Tag::new(format!("Tag {}", i));
            store.create_tag(&tag).unwrap();

            // Assign to stations
            for j in 0..2 {
                let station_tag = StationTag::new(format!("station-{}", j), tag.id);
                store.assign_tag(&station_tag).unwrap();
            }
        }

        let stats = store.stats();
        assert_eq!(stats.total_tags, 3);
        assert_eq!(stats.total_assignments, 6);

        // Clean up
        std::fs::remove_file(db_path).ok();
    }
}