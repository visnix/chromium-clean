// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//! Configures gnrt behavior. Types match `gnrt_config.toml` fields. Currently
//! only used for std bindings.

use std::collections::BTreeMap;

use serde::Deserialize;

/// For list-type configuration fields, combine the configuration entries with
/// the same field name from a particular crate's config with the global config.
/// This allows for a global setting to be applied to every crate, with
/// individual crates extending it.
///
/// The result is an iterator over all config values. The crate config will come
/// first, then global configs will come later in the list.
macro_rules! config_field {
    ($config:expr, $name:expr, $field:ident, $default_config:expr) => {
        $config
            .per_crate_config
            .get($name)
            .or_else(|| Some($default_config))
            .map(|crate_config| {
                crate::config::do_concat_field(|c| &c.$field, &crate_config, &$config.all_config)
            })
            .unwrap()
    };
}
pub(crate) use config_field;

/// Combine a field from `crate_config` and `all_config`, in order. This can be
/// used to combine config lists, or get the first set `Option<_>` of the two
/// configs.
pub fn do_concat_field<
    'a,
    T: 'a,
    Field: 'a + IntoIterator<Item = T>,
    F: Fn(&'a CrateConfig) -> Field,
>(
    field_mapper: F,
    crate_config: &'a CrateConfig,
    all_config: &'a CrateConfig,
) -> impl Iterator<Item = T> {
    field_mapper(crate_config).into_iter().chain(field_mapper(all_config).into_iter())
}

#[derive(Clone, Debug, Default, Deserialize)]
#[serde(default, deny_unknown_fields)]
pub struct BuildConfig {
    pub resolve: ResolveConfig,
    /// Configuration that applies to all crates
    #[serde(rename = "all-crates")]
    pub all_config: CrateConfig,
    /// Additional configuration options for specific crates. Keyed by crate
    /// name. Config is additive with `all_config`.
    #[serde(rename = "crate")]
    pub per_crate_config: BTreeMap<String, CrateConfig>,
}

/// Influences dependency resolution for a session.
#[derive(Clone, Debug, Default, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct ResolveConfig {
    /// The Cargo package to use as the root of the dependency graph.
    pub root: String,
    /// Remove crates from the set of resolved dependencies. Should be used
    /// sparingly; it does not affect Cargo's dependency resolution, so the
    /// output can easily be incorrect. This is primarily intended to work
    /// around bugs in `cargo metadata` output.
    pub remove_crates: Vec<String>,
}

/// Customizes GN output for a crate.
#[derive(Clone, Debug, Default, Deserialize)]
#[serde(default, deny_unknown_fields)]
pub struct CrateConfig {
    /// `cfg(...)` options for building this crate.
    pub cfg: Vec<String>,
    /// Features to enable when building this crate.
    pub features: Vec<String>,
    /// Dependencies to remove from this crate. Note that this happens after
    /// dependency and feature resolution, so if an optional dep's feature is
    /// enabled but the dep is removed, the crate will still attempt to
    /// reference that dependency.
    pub remove_deps: Vec<String>,
    /// Compile-time environment variables for this crate.
    pub env: Vec<String>,
    /// Apply rustc metadata to this target.
    pub rustc_metadata: Option<String>,
    /// Extra rustc flags.
    pub rustflags: Vec<String>,
    /// Sets GN output_dir variable.
    pub output_dir: Option<String>,
    /// Adds the specified default library configs in the target.
    #[serde(default)]
    pub add_library_configs: Vec<String>,
    /// Removes the specified default library configs in the target.
    #[serde(default)]
    pub remove_library_configs: Vec<String>,
    /// Skip building and running the build script.
    pub skip_build_rs: bool,
    /// GN deps to add to the generated target.
    pub extra_gn_deps: Vec<String>,
    /// Remove GN deps added by the overall config.
    ///
    /// TODO(crbug.com/1245714): find a way to express these sorts of
    /// dependencies.
    #[serde(default)]
    pub extra_gn_deps_to_ignore: Vec<String>,
    /// Deps on generated targets to exclude from this target's `deps` list.
    /// These do not affect dependency resolution, so it will not change any
    /// other generated targets.
    pub exclude_deps_in_gn: Vec<String>,
    /// Include rs and input files under these relative paths as part of the
    /// crate.
    #[serde(default)]
    pub extra_src_roots: Vec<std::path::PathBuf>,
    /// Include input files under these relative paths as part of the crate.
    #[serde(default)]
    pub extra_input_roots: Vec<std::path::PathBuf>,
}
