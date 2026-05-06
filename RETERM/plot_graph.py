import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams["font.family"] = "Times New Roman"

df = pd.read_csv(
    "Default_Dataset.csv",
    header=None,
    names=["F_Hz", "A_ms2"]
)

df = df.sort_values("F_Hz").reset_index(drop=True)

fig, ax = plt.subplots(figsize=(6, 5))

ax.plot(df["F_Hz"], df["A_ms2"], linewidth=1.5, color="#1f77b4")

ax.set_xlabel("F (Hz)", fontsize=14)
ax.set_ylabel("A (m/s²)", fontsize=14)
ax.tick_params(axis="both", labelsize=12)
ax.set_xlim(0, 100)
ax.set_ylim(0)
ax.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)

plt.tight_layout()
plt.savefig("espectro_frequencia.png", dpi=300)
print("Salvo.")