"""
General Morphological Analysis (GMA)
Classes: Parameter, MorphologicalField, CrossConsistencyMatrix, GMAModel
Visualizations: Morphological Field table, Cross-Consistency Matrix
"""

import itertools
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch
import numpy as np


# ─────────────────────────────────────────────
# Core Classes
# ─────────────────────────────────────────────

class Parameter:
    def __init__(self, name: str, values: list[str]):
        self.name = name
        self.values = values

    def __repr__(self):
        return f"Parameter('{self.name}', {self.values})"


class MorphologicalField:
    def __init__(self, parameters: list[Parameter]):
        self.parameters = parameters

    def configuration_space(self) -> list[tuple]:
        """Returns the full Cartesian product of all parameter values."""
        return list(itertools.product(*[p.values for p in self.parameters]))

    def total_configurations(self) -> int:
        result = 1
        for p in self.parameters:
            result *= len(p.values)
        return result


class CrossConsistencyMatrix:
    def __init__(self, field: MorphologicalField):
        self.field = field
        # inconsistencies: set of frozensets {(param_name, value), (param_name, value)}
        self.inconsistencies: list[tuple[tuple, tuple]] = []

    def add_inconsistency(self, param_a: str, value_a: str, param_b: str, value_b: str):
        """Register a pairwise inconsistency between two parameter values."""
        self.inconsistencies.append(((param_a, value_a), (param_b, value_b)))

    def is_consistent(self, config: tuple) -> bool:
        """Check whether a configuration violates any registered inconsistency."""
        param_names = [p.name for p in self.field.parameters]
        config_dict = {param_names[i]: config[i] for i in range(len(config))}

        for (pa, va), (pb, vb) in self.inconsistencies:
            if config_dict.get(pa) == va and config_dict.get(pb) == vb:
                return False
        return True

    def filter(self) -> list[tuple]:
        """Return only the configurations that pass all consistency checks."""
        return [c for c in self.field.configuration_space() if self.is_consistent(c)]


class GMAModel:
    def __init__(self, name: str, field: MorphologicalField, ccm: CrossConsistencyMatrix):
        self.name = name
        self.field = field
        self.ccm = ccm

    def report(self):
        total = self.field.total_configurations()
        viable = self.ccm.filter()
        eliminated = total - len(viable)
        reduction = (eliminated / total) * 100

        print(f"\n{'='*50}")
        print(f"GMA Model: {self.name}")
        print(f"{'='*50}")
        print(f"\nParameters:")
        for p in self.field.parameters:
            print(f"  {p.name}: {p.values}")
        print(f"\nTotal configurations (k): {total}")
        print(f"Inconsistencies registered: {len(self.ccm.inconsistencies)}")
        print(f"Viable configurations: {len(viable)}")
        print(f"Eliminated: {eliminated} ({reduction:.1f}%)")
        print(f"\nViable configurations:")
        for c in viable:
            print(f"  {c}")

    def plot_morphological_field(self, save_path: str = None):
        """Render the morphological field as a table (style: Kempf 2015)."""
        params = self.field.parameters
        max_values = max(len(p.values) for p in params)
        n_params = len(params)

        fig_width = max(10, n_params * 2.2)
        fig_height = max(3, (max_values + 1) * 0.55)
        fig, ax = plt.subplots(figsize=(fig_width, fig_height))
        ax.set_xlim(0, n_params)
        ax.set_ylim(-0.1, max_values + 1)
        ax.axis('off')

        col_w = 1.0
        header_h = 0.7
        cell_h = 0.55

        header_color = '#d0d0d0'
        cell_color = '#ffffff'
        border_color = '#555555'

        # top_y: y coordinate where the last value row ends (top of value area)
        top_y = max_values * cell_h

        for col, param in enumerate(params):
            x = col * col_w

            # Header — placed immediately above the value area
            header_y = top_y + 0.02
            rect = FancyBboxPatch(
                (x + 0.02, header_y),
                col_w - 0.04, header_h - 0.1,
                boxstyle="square,pad=0",
                linewidth=0.8, edgecolor=border_color, facecolor=header_color
            )
            ax.add_patch(rect)
            ax.text(
                x + col_w / 2, header_y + (header_h - 0.1) / 2,
                param.name, ha='center', va='center',
                fontsize=8, fontweight='bold', wrap=True
            )

            # Values
            for row, val in enumerate(param.values):
                y = (max_values - 1 - row) * cell_h
                rect = FancyBboxPatch(
                    (x + 0.02, y),
                    col_w - 0.04, cell_h - 0.05,
                    boxstyle="square,pad=0",
                    linewidth=0.6, edgecolor=border_color, facecolor=cell_color
                )
                ax.add_patch(rect)
                ax.text(
                    x + col_w / 2, y + (cell_h - 0.05) / 2,
                    val, ha='center', va='center', fontsize=7.5
                )

        fig.suptitle(f"Morphological Field — {self.name}", fontsize=10, fontweight='bold', y=0.98)
        plt.tight_layout()

        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Saved: {save_path}")
        plt.show()

    def plot_cross_consistency_matrix(self, save_path: str = None):
        """Render the upper-triangular cross-consistency matrix with red squares and group separators."""
        params = self.field.parameters

        labels = []
        param_labels = []
        for p in params:
            for v in p.values:
                labels.append((p.name, v))
            param_labels.append((p.name, len(p.values)))

        n = len(labels)
        # Last parameter never appears as an active row — exclude it
        n_rows = n - len(params[-1].values)

        incon_set = set()
        for (pa, va), (pb, vb) in self.ccm.inconsistencies:
            incon_set.add(((pa, va), (pb, vb)))
            incon_set.add(((pb, vb), (pa, va)))

        cell = 0.65
        val_label_w = 1.8   # width for value labels (rows)
        grp_label_w = 0.5   # width for group labels (rows)
        val_label_h = 1.5   # height for value labels (cols)
        grp_label_h = 0.5   # height for group labels (cols)
        margin = 0.2

        # First parameter never appears as an active column — exclude it
        n_cols_skip = len(params[0].values)
        col_labels = labels[n_cols_skip:]
        col_start_x = n_cols_skip * cell  # x offset where active columns begin

        grid_w = n * cell
        grid_h = n_rows * cell
        active_grid_w = (n - n_cols_skip) * cell
        fig_w = grp_label_w + val_label_w + active_grid_w + margin
        fig_h = grp_label_h + val_label_h + grid_h + margin

        fig, ax = plt.subplots(figsize=(fig_w, fig_h))

        # Shift x origin so active columns start at 0
        ax.set_xlim(-(grp_label_w + val_label_w), active_grid_w + margin)
        ax.set_ylim(-(grid_h + margin), grp_label_h + val_label_h)
        ax.axis('off')

        border      = '#555555'
        border_grp  = '#222222'
        gray        = '#cccccc'
        light       = '#f5f5f5'
        incon_color = '#fd3f36'

        param_of = {(p.name, v): p.name for p in params for v in p.values}

        # ── Grid cells ──────────────────────────────────────────────────────
        for row in range(n_rows):
            for col_idx, label_col in enumerate(col_labels):
                x = col_idx * cell
                y = -(row + 1) * cell
                prow = param_of[labels[row]]
                pcol = param_of[label_col]
                # original col index in full labels list
                orig_col = col_idx + n_cols_skip
                inactive = (orig_col <= row) or (prow == pcol)

                rect = patches.Rectangle(
                    (x, y), cell, cell,
                    linewidth=0.4, edgecolor=border,
                    facecolor=gray if inactive else (incon_color if (labels[row], label_col) in incon_set else light)
                )
                ax.add_patch(rect)

        # ── Parameter group separators (thick borders) ───────────────────
        # Columns — skip first param group
        x_cursor = 0
        for i, (_, count) in enumerate(param_labels):
            if i == 0:
                x_cursor += count * cell
                continue
            gx = x_cursor - n_cols_skip * cell
            ax.plot([gx, gx], [-grid_h, 0], color=border_grp, linewidth=1.5)
            x_cursor += count * cell
        ax.plot([active_grid_w, active_grid_w], [-grid_h, 0], color=border_grp, linewidth=1.5)
        ax.plot([0, 0], [-grid_h, 0], color=border_grp, linewidth=1.5)

        # Rows — skip last param group
        y_cursor = 0
        for _, count in param_labels[:-1]:
            gy = -y_cursor
            ax.plot([0, active_grid_w], [gy, gy], color=border_grp, linewidth=1.5)
            y_cursor += count * cell
        ax.plot([0, active_grid_w], [-grid_h, -grid_h], color=border_grp, linewidth=1.5)

        # ── Value labels — columns (rotated), skip first param ───────────────
        for col_idx, (_, val) in enumerate(col_labels):
            x = col_idx * cell + cell / 2
            ax.text(x, 0.05, val,
                    ha='left', va='bottom', fontsize=11,
                    rotation=90, rotation_mode='anchor')

        # ── Value labels — rows (only n_rows) ───────────────────────────────
        for row, (_, val) in enumerate(labels[:n_rows]):
            y = -(row + 0.5) * cell
            ax.text(-0.1, y, val,
                    ha='right', va='center', fontsize=11)

        # ── Parameter group headers — columns (skip first) ──────────────────
        x_cursor = 0
        col_header_y = val_label_h + grp_label_h / 2
        for i, (pname, count) in enumerate(param_labels):
            if i == 0:
                x_cursor += count * cell
                continue
            mid = (x_cursor - n_cols_skip * cell) + count * cell / 2
            ax.text(mid, col_header_y, pname,
                    ha='center', va='center', fontsize=12,
                    fontweight='bold', rotation=0)
            x_cursor += count * cell

        # ── Parameter group headers — rows (skip last) ──────────────────────
        y_cursor = 0
        for pname, count in param_labels[:-1]:
            mid = -(y_cursor + count * cell / 2)
            ax.text(-(val_label_w - 0.1), mid, pname,
                    ha='center', va='center', fontsize=12, fontweight='bold',
                    rotation=90)
            y_cursor += count * cell

        fig.suptitle(f"Cross-Consistency Matrix — {self.name}",
                     fontsize=14, fontweight='bold', y=0.98)
        plt.tight_layout()

        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Saved: {save_path}")
        plt.show()


# ─────────────────────────────────────────────
# Example: Perception Sensor Selection for AV L3
# ─────────────────────────────────────────────

if __name__ == "__main__":

    # Parameters
    p1 = Parameter("Modalidade", ["Câmera", "LiDAR", "Radar"])
    p2 = Parameter("Alcance", ["Curto (<30m)", "Médio (30-100m)", "Longo (>100m)"])
    p3 = Parameter("Cond. Operação", ["Boa visib.", "Baixa visib.", "Noite"])
    p4 = Parameter("Custo", ["Baixo (<$500)", "Médio ($500-5k)", "Alto (>$5k)"])
    p5 = Parameter("Resolução", ["Baixa", "Média", "Alta"])

    field = MorphologicalField([p1, p2, p3, p4, p5])

    # Cross-Consistency Matrix
    ccm = CrossConsistencyMatrix(field)

    # P1 x P3
    ccm.add_inconsistency("Modalidade", "Câmera", "Cond. Operação", "Baixa visib.")
    ccm.add_inconsistency("Modalidade", "Câmera", "Cond. Operação", "Noite")

    # P1 x P2
    ccm.add_inconsistency("Modalidade", "LiDAR", "Alcance", "Longo (>100m)")

    # P1 x P4
    ccm.add_inconsistency("Modalidade", "LiDAR", "Custo", "Baixo (<$500)")
    ccm.add_inconsistency("Modalidade", "Radar", "Custo", "Alto (>$5k)")
    ccm.add_inconsistency("Modalidade", "Câmera", "Custo", "Alto (>$5k)")

    # P1 x P5
    ccm.add_inconsistency("Modalidade", "Radar", "Resolução", "Alta")
    ccm.add_inconsistency("Modalidade", "Câmera", "Resolução", "Baixa")

    # P2 x P4
    ccm.add_inconsistency("Alcance", "Longo (>100m)", "Custo", "Baixo (<$500)")

    # P2 x P5
    ccm.add_inconsistency("Alcance", "Longo (>100m)", "Resolução", "Alta")

    # P4 x P5
    ccm.add_inconsistency("Custo", "Baixo (<$500)", "Resolução", "Alta")
    ccm.add_inconsistency("Custo", "Alto (>$5k)", "Resolução", "Baixa")

    # Model
    model = GMAModel("Sensor de Percepção — AV Nível 3", field, ccm)

    # Report
    model.report()

    # Plots
    model.plot_morphological_field(save_path="morphological_field.png")
    model.plot_cross_consistency_matrix(save_path="cross_consistency_matrix.png")