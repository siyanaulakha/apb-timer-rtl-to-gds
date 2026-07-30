# Frozen Sky130 Signoff Evidence

This directory records the compact evidence for the final run:

`apb_timer_sky130_signoff_policy_only_20260730_021912`

Committed evidence includes the final metrics, signoff summary, exact run configuration, implementation/signoff SDCs, warning audit, and checksums for the larger local views.

The following view directories are intentionally ignored by Git because they are generated and comparatively large:

```text
def/  gds/  lef/  lib/  nl/  pnl/  sdc/  sdf/  spef/
```

On the implementation workstation, verify those views with:

```bash
cd results/final-openlane
sha256sum -c LOCAL_VIEW_SHA256SUMS
```

The final GDSII checksum is:

```text
078404297dd47556bbca79b8a3427e56715b83863e97065b540eaee7c7a88302  gds/apb_timer.gds
```

Create a portable release bundle with:

```bash
./scripts/package_final_openlane.sh
```

The generated archive under `dist/` should be uploaded as a GitHub Release asset. A fresh clone will not contain the ignored physical views until that bundle is downloaded and extracted.
