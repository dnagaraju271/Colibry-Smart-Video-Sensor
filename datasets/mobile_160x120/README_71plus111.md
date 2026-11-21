# Deduplicated 71 + 111 walking-scene dataset

Current archive:
`OWN_Dataset_71plus111_Deduplicated_Scene_Frames_160x120.zip`

The source collections contained 71 legacy processed images and 111 newer
timestamped outdoor images (182 records total). Exact pixel comparison after
160x120 grayscale normalization found every legacy image duplicated in the
newer collection. The legacy set contributed zero unique images. The current
archive therefore retains 111 unique frames and records all 71 legacy
duplicates in `OWN_Dataset_71plus111_duplicates_removed.csv`.

Class counts:
- 0 people: 1 (preserved, skipped by the four-output model)
- 1 person: 13
- 2 people: 58
- 3 people: 25
- 4 people: 14

All images are 160x120 grayscale PNGs, split into matching
`0_person/` through `4_person/` folders. Naming convention:
`I{count}_{scene}_{frame}.png`. Frame numbers increase across all class
folders within the same timestamped scene.

There are 14 source timestamp scenes. Six longer reviewed scenes are marked
as bidirectional with entry and exit on both sides; eight shorter scenes are
marked UNKNOWN because the available stills do not establish direction.
These are scene-level annotations, not person-level tracking labels.

The ZIP includes `manifest.csv`, `scene_directions.csv`, `scene_summary.csv`,
`duplicates_removed.csv`, `source_reconciliation.csv`, and this README.
