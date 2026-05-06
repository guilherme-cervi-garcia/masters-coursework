"""
FDIIR Morphological Field — Selection Approaches Comparison
===========================================================
Implements the 4-condition factorial design (D02):
  Baseline : MA básica — admissible set, no ranking
  A        : Scoring linear ponderado (Vetor A active)
  B        : Weighted CCA — IC weights (Vetor B active)  [structural placeholder]
  A+B      : Both vectors active

Abordagens de seleção (E02):
  1. Scoring linear ponderado
  2. Ideal point
  3. Pareto-based MA
"""

import json
import itertools
import math
from copy import deepcopy

# ── 1. CAMPO MORFOLÓGICO ─────────────────────────────────────────────────────

PARAMS = ["D", "I", "ID", "R"]

VALUES = {
    "D":  ["modelo", "sinal", "dados", "diagnostico_ativo"],
    "I":  ["comparacao_saidas", "votacao", "modelagem_dependencias", "filtros_adaptativos"],
    "ID": ["regras", "estatistico", "ML"],
    "R":  ["exclusao", "reconf_pesos", "bypass"],
}

CRITERIA = ["C1_interpretabilidade", "C2_custo_inv", "C3_latencia_inv",
            "C4_maturidade", "C5_cobertura"]

# ── 2. MATRIZ DE SCORES: valores × critérios (D20) ───────────────────────────

SCORES = {
    # D — Método de Detecção
    ("D", "modelo"):             [5, 4, 4, 5, 3],
    ("D", "sinal"):              [5, 5, 5, 4, 4],
    ("D", "dados"):              [2, 2, 3, 3, 5],
    ("D", "diagnostico_ativo"):  [3, 2, 1, 3, 5],
    # I — Método de Isolamento
    ("I", "comparacao_saidas"):       [5, 5, 5, 5, 3],
    ("I", "votacao"):                 [4, 4, 4, 5, 4],
    ("I", "modelagem_dependencias"):  [3, 2, 2, 3, 5],
    ("I", "filtros_adaptativos"):     [3, 3, 3, 3, 4],
    # ID — Método de Identificação
    ("ID", "regras"):      [5, 5, 5, 4, 3],
    ("ID", "estatistico"): [4, 4, 4, 5, 4],
    ("ID", "ML"):          [2, 2, 3, 3, 5],
    # R — Método de Recuperação
    ("R", "exclusao"):     [5, 5, 5, 5, 3],
    ("R", "reconf_pesos"): [4, 3, 3, 4, 5],
    ("R", "bypass"):       [3, 2, 2, 2, 4],
}

# ── 3. ESPAÇO DE SOLUÇÕES (D18 — instância sintética) ────────────────────────

with open("solution_space.json") as f:
    _raw = json.load(f)

SOLUTIONS = {s["id"]: (s["D"], s["I"], s["ID"], s["R"]) for s in _raw}

# ── 4. CENÁRIOS DE PESO — VETOR A (D02 / E02) ────────────────────────────────
# Cada cenário: w_param (4) + w_crit (5), todos somam 1

SCENARIOS = {
    "SC1_uniforme": {
        "w_param": {"D": 0.25, "I": 0.25, "ID": 0.25, "R": 0.25},
        "w_crit":  [0.20, 0.20, 0.20, 0.20, 0.20],
        "label": "Uniforme (baseline)"
    },
    "SC2_seguranca": {
        "w_param": {"D": 0.35, "I": 0.25, "ID": 0.25, "R": 0.15},
        "w_crit":  [0.35, 0.10, 0.10, 0.35, 0.10],
        "label": "Segurança / certificação"
    },
    "SC3_cobertura": {
        "w_param": {"D": 0.30, "I": 0.20, "ID": 0.20, "R": 0.30},
        "w_crit":  [0.10, 0.20, 0.20, 0.10, 0.40],
        "label": "Cobertura / desempenho"
    },
    "SC4_eficiencia": {
        "w_param": {"D": 0.25, "I": 0.25, "ID": 0.25, "R": 0.25},
        "w_crit":  [0.10, 0.35, 0.35, 0.10, 0.10],
        "label": "Eficiência de recursos"
    },
}

# ── 5. FUNÇÕES AUXILIARES ─────────────────────────────────────────────────────

def score_value(param, val, w_crit):
    """Score ponderado de um valor num parâmetro dado w_crit."""
    raw = SCORES[(param, val)]
    return sum(w * s for w, s in zip(w_crit, raw))

def score_config(sid, scenario):
    """Score total de uma configuração num cenário (scoring linear ponderado)."""
    config = SOLUTIONS[sid]
    wp = scenario["w_param"]
    wc = scenario["w_crit"]
    total = 0.0
    for param, val in zip(PARAMS, config):
        total += wp[param] * score_value(param, val, wc)
    return round(total, 4)

def ideal_point_scores(w_crit=None):
    """Score agregado do ponto ideal por parâmetro (melhor valor em cada param)."""
    if w_crit is None:
        w_crit = [0.2] * 5
    ideal = {}
    for param in PARAMS:
        ideal[param] = max(score_value(param, v, w_crit) for v in VALUES[param])
    return ideal

def distance_to_ideal(sid, w_crit=None):
    """Distância euclidiana de uma configuração ao ponto ideal."""
    if w_crit is None:
        w_crit = [0.2] * 5
    ideal = ideal_point_scores(w_crit)
    config = SOLUTIONS[sid]
    dist = 0.0
    for param, val in zip(PARAMS, config):
        s = score_value(param, val, w_crit)
        dist += (s - ideal[param]) ** 2
    return round(math.sqrt(dist), 4)

def config_criterion_vector(sid, w_crit=None):
    """Vetor de scores por critério de uma configuração (para Pareto)."""
    if w_crit is None:
        w_crit = [0.2] * 5
    config = SOLUTIONS[sid]
    # Score por critério: soma ponderada pelos parâmetros (pesos uniformes)
    crit_scores = [0.0] * 5
    for param, val in zip(PARAMS, config):
        raw = SCORES[(param, val)]
        for i, s in enumerate(raw):
            crit_scores[i] += s / len(PARAMS)
    return [round(s, 4) for s in crit_scores]

def dominates(v1, v2):
    """v1 domina v2 se melhor ou igual em todos os critérios e estritamente melhor em algum."""
    better_in_any = False
    for a, b in zip(v1, v2):
        if a < b:
            return False
        if a > b:
            better_in_any = True
    return better_in_any

def pareto_front(solution_ids, w_crit=None):
    """Retorna IDs das configurações Pareto-eficientes."""
    vectors = {sid: config_criterion_vector(sid, w_crit) for sid in solution_ids}
    pareto = []
    for sid in solution_ids:
        dominated = False
        for other in solution_ids:
            if other != sid and dominates(vectors[other], vectors[sid]):
                dominated = True
                break
        if not dominated:
            pareto.append(sid)
    return pareto

# ── 6. EXPERIMENTOS ───────────────────────────────────────────────────────────

def run_all():
    solution_ids = list(SOLUTIONS.keys())
    results = {}

    print("=" * 70)
    print("FDIIR — COMPARAÇÃO DE ABORDAGENS DE SELEÇÃO")
    print("Espaço de soluções: {} configurações admissíveis".format(len(solution_ids)))
    print("=" * 70)

    # ── CONDIÇÃO BASELINE: MA básica ─────────────────────────────────────────
    print("\n── BASELINE: MA básica (sem ranqueamento) ──────────────────────────")
    print("Conjunto admissível: " + ", ".join(solution_ids))
    results["baseline"] = {"admissible": solution_ids}

    # ── CONDIÇÃO A: Scoring linear ponderado ─────────────────────────────────
    print("\n── CONDIÇÃO A: Scoring Linear Ponderado ────────────────────────────")
    results["A"] = {}
    for sc_id, sc in SCENARIOS.items():
        ranked = sorted(solution_ids, key=lambda s: score_config(s, sc), reverse=True)
        scores = {s: score_config(s, sc) for s in solution_ids}
        results["A"][sc_id] = {"ranking": ranked, "scores": scores}
        print(f"\n  {sc_id} — {sc['label']}")
        for rank, sid in enumerate(ranked, 1):
            config = SOLUTIONS[sid]
            print(f"    {rank:2d}. {sid}  Q={scores[sid]:.4f}  "
                  f"D={config[0]:<22} I={config[1]:<28} "
                  f"ID={config[2]:<12} R={config[3]}")

    # ── CONDIÇÃO B: Ideal Point ───────────────────────────────────────────────
    # (Vetor B — IC weights — estrutura reservada; aqui usa w_crit uniforme)
    print("\n── CONDIÇÃO B: Ideal Point ─────────────────────────────────────────")
    w_crit_uniform = [0.2] * 5
    ideal = ideal_point_scores(w_crit_uniform)
    print(f"  Ponto ideal por parâmetro (w_crit uniforme):")
    for p, s in ideal.items():
        best_val = max(VALUES[p], key=lambda v: score_value(p, v, w_crit_uniform))
        print(f"    {p}: {best_val} (score={s:.2f})")

    ranked_ip = sorted(solution_ids, key=lambda s: distance_to_ideal(s, w_crit_uniform))
    distances = {s: distance_to_ideal(s, w_crit_uniform) for s in solution_ids}
    results["B"] = {"ranking": ranked_ip, "distances": distances,
                    "ideal": ideal}
    print(f"\n  Ranking por distância ao ideal:")
    for rank, sid in enumerate(ranked_ip, 1):
        config = SOLUTIONS[sid]
        print(f"    {rank:2d}. {sid}  ρ={distances[sid]:.4f}  "
              f"D={config[0]:<22} I={config[1]:<28} "
              f"ID={config[2]:<12} R={config[3]}")

    # ── CONDIÇÃO A+B: Pareto-based MA ─────────────────────────────────────────
    print("\n── CONDIÇÃO A+B: Pareto-based MA ───────────────────────────────────")
    pareto = pareto_front(solution_ids, w_crit_uniform)
    non_pareto = [s for s in solution_ids if s not in pareto]
    results["AB"] = {"pareto_front": pareto, "dominated": non_pareto}

    print(f"  Conjunto Pareto-eficiente ({len(pareto)} configurações):")
    for sid in pareto:
        config = SOLUTIONS[sid]
        vec = config_criterion_vector(sid, w_crit_uniform)
        print(f"    {sid}  C={vec}  "
              f"D={config[0]:<20} I={config[1]:<26} "
              f"ID={config[2]:<10} R={config[3]}")

    print(f"\n  Configurações dominadas ({len(non_pareto)}):")
    print(f"    " + ", ".join(non_pareto))

    # ── ANÁLISE DE SENSIBILIDADE ──────────────────────────────────────────────
    print("\n── ANÁLISE DE SENSIBILIDADE (Condição A) ───────────────────────────")
    print("  Estabilidade do top-3 entre cenários:")
    top3_per_sc = {}
    for sc_id in SCENARIOS:
        top3_per_sc[sc_id] = results["A"][sc_id]["ranking"][:3]
        print(f"  {sc_id}: {top3_per_sc[sc_id]}")

    all_top3 = set(s for t in top3_per_sc.values() for s in t)
    stable = [s for s in all_top3
              if all(s in top3_per_sc[sc] for sc in SCENARIOS)]
    print(f"\n  Configurações no top-3 em TODOS os cenários: "
          f"{stable if stable else 'nenhuma'}")
    print(f"  Configurações que aparecem em pelo menos 1 top-3: "
          f"{sorted(all_top3)}")

    # Spearman entre SC1 e SC2
    def spearman(r1, r2):
        n = len(r1)
        d2 = sum((r1.index(s) - r2.index(s))**2 for s in r1)
        return round(1 - 6*d2 / (n*(n**2-1)), 4)

    pairs = list(itertools.combinations(list(SCENARIOS.keys()), 2))
    print("\n  Correlação de Spearman entre rankings:")
    for sc1, sc2 in pairs:
        r1 = results["A"][sc1]["ranking"]
        r2 = results["A"][sc2]["ranking"]
        rho = spearman(r1, r2)
        print(f"    {sc1} × {sc2}: ρ = {rho}")

    # ── SALVAR RESULTADOS ─────────────────────────────────────────────────────
    with open("results.json", "w") as f:
        json.dump(results, f, indent=2, ensure_ascii=False)
    print("\nResultados salvos em results.json")

    return results

if __name__ == "__main__":
    run_all()
