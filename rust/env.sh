
export CARGO_HOME=/data/cargo
export RUSTUP_HOME=/data/rustup
toolchain_dir=${RUSTUP_HOME}/toolchains
toolchain=stable-aarch64-unknown-linux-gnu
bin_dir=${toolchain_dir}/${toolchain}/bin

# TODO: have tmux make a new window with these env vars
# for doing rust stuff
export PATH="${PATH}:${bin_dir}"

# http bind address (addy on the ssl cert too)
export BIND="192.168.1.2"
