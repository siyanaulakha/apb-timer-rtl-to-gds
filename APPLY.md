# Apply v0.1.2

Extract this archive directly over the existing repository:

```bash
unzip -o "$HOME/Downloads/apb-timer-v0.1.2-fix.zip" \
  -d "$HOME/research/projects/apb-timer-rtl-to-gds"

cd "$HOME/research/projects/apb-timer-rtl-to-gds"
chmod +x scripts/*.sh
```

Then run:

```bash
source "$HOME/tools/eda/oss-cad-suite/environment"
make clean
make verify RANDOM_OPERATIONS=500 SEED=107
make coverage RANDOM_OPERATIONS=500 SEED=107
make results
```
