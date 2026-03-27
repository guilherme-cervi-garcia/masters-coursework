# General Morphological Analysis (GMA)

Python implementation of General Morphological Analysis for structured exploration of complex problem spaces. Developed as part of an M.Sc. dissertation on FDIIR frameworks for autonomous vehicle perception systems at UFSC.

## Dependencies

```bash
pip install matplotlib numpy
```

## Structure

The implementation is organized into four classes:

**`Parameter`** — defines a single dimension of the problem space, with a name and a list of possible values (variants).

**`MorphologicalField`** — composes a list of `Parameter` objects and computes the full Cartesian product of their value sets, generating the formal configuration space.

**`CrossConsistencyMatrix`** — registers pairwise inconsistencies between parameter values and filters the configuration space, retaining only viable configurations.

**`GMAModel`** — composes `MorphologicalField` and `CrossConsistencyMatrix`, generates a text report, and renders two visualizations.

## Usage

```python
from gma import Parameter, MorphologicalField, CrossConsistencyMatrix, GMAModel

# 1. Define parameters independently
p1 = Parameter("Sensor", ["Camera", "LiDAR", "Radar"])
p2 = Parameter("Range",  ["Short", "Medium", "Long"])
p3 = Parameter("Cost",   ["Low", "Medium", "High"])

# 2. Build the morphological field
field = MorphologicalField([p1, p2, p3])

# 3. Register pairwise inconsistencies
ccm = CrossConsistencyMatrix(field)
ccm.add_inconsistency("Sensor", "Camera", "Cost", "High")
ccm.add_inconsistency("Sensor", "LiDAR",  "Range", "Long")
ccm.add_inconsistency("Cost",   "Low",    "Range", "Long")

# 4. Build the model
model = GMAModel("My Problem", field, ccm)

# 5. Generate report and visualizations
model.report()
model.plot_morphological_field(save_path="morphological_field.png")
model.plot_cross_consistency_matrix(save_path="cross_consistency_matrix.png")
```

## Output

`model.report()` prints to stdout:

```
==================================================
GMA Model: My Problem
==================================================

Parameters:
  Sensor: ['Camera', 'LiDAR', 'Radar']
  Range: ['Short', 'Medium', 'Long']
  Cost: ['Low', 'Medium', 'High']

Total configurations (k): 27
Inconsistencies registered: 3
Viable configurations: 18
Eliminated: 9 (33.3%)

Viable configurations:
  ('Camera', 'Short', 'Low')
  ...
```

`plot_morphological_field()` renders a table with parameter headers and value cells.

`plot_cross_consistency_matrix()` renders an upper-triangular matrix where inconsistent pairs are highlighted in red. The first parameter column and last parameter row are omitted, as they contain no active cells in the triangular layout.

Both methods accept an optional `save_path` argument. If omitted, the plot is displayed but not saved.

## Design Principles

Inconsistencies must be registered based on domain knowledge, independently of the visualization. The morphological field is defined before the CCA — variants should represent plausible options for the problem, not entries chosen to produce inconsistencies.

The three types of inconsistency supported (following Ritchey 2015) are:

- **Logical** — analytic contradiction between two values
- **Empirical** — incompatibility based on physical or technical constraints
- **Normative** — constraint based on cost, policy, or design rationale

## Reference

> Ritchey, T. (2015). Principles of Cross-Consistency Assessment in General Morphological Modelling. *Acta Morphologica Generalis*, 4(2).

> Kempf, M. (2015). *General Morphological Analysis: A general method for non-quantified modelling*. Swedish Morphological Society.