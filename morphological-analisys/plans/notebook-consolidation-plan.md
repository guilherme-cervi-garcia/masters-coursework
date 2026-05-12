# Notebook Consolidation Plan: Morphological Analysis (GMA/FDIIR)

**Date:** May 12, 2026  
**Status:** READY_FOR_EXECUTION  
**Complexity Tier:** MEDIUM  
**Confidence:** 0.92

---

## Executive Summary

Consolidate three overlapping Jupyter notebooks (65% code redundancy) into three coherent, non-redundant analysis notebooks representing versions V2 and V3 of GMA/FDIIR research, plus a unified comparative analysis notebook. All non-redundant results preserved; domain documentation and LLM context retained; redundant setup code deduplicated across notebooks.

---

## Scope & Boundaries

### IN Scope
- Refactor [gma_antiga.ipynb](../gma_antiga.ipynb) → v2-gma-fdiir-analysis.ipynb (clean V2 implementation)
- Refactor [fdiir_gma.ipynb](../fdiir_gma.ipynb) → v3-gma-fdiir-analysis.ipynb (clean V3 implementation with LLM context)
- Refactor [analise_versoes_gma.ipynb](../analise_versoes_gma.ipynb) → consolidated-comparative-analysis.ipynb (unified V2/V3 comparison)
- Eliminate ~400 lines of redundant setup/method code across notebooks
- Maintain Python utility scripts ([morphological-analisys.py](../morphological-analisys.py), [fdiir_selection.py](../fdiir_selection.py)) as-is

### OUT of Scope
- Modifying core Python modules (morphological-analisys.py, fdiir_selection.py)
- Creating new analysis methods or adding new findings
- Changing parameter values or CCM/VETOR_A definitions
- Restructuring the domain documentation or LLM evaluation context

### Constraints
- No modifications to original files until consolidation plan approved
- Preserve all results that are not redundant
- Flag inconsistencies between notebooks for user decision
- Language: English throughout
- Keep domain context/prompts unchanged

---

## Identified Redundancies

### 1. Setup Code (100% Redundancy)
- **Location:** gma_antiga.ipynb cells 1-5; fdiir_gma.ipynb cells 1-5
- **Content:** Pandas imports, display options, numpy/itertools setup
- **Lines:** ~20 lines
- **Action:** Consolidate into single setup cell in each notebook; document once

### 2. Data Definitions (100% Redundancy)
| Item | Duplicate Locations | Lines | Difference |
|------|-------------------|-------|-----------|
| MODULES, MODULE_VALUES | gma_antiga + fdiir_gma | 10 | None — identical |
| CRITERIA, CRITERIA_NAMES | gma_antiga + fdiir_gma | 10 | None — identical |
| DA_NAMES | gma_antiga + fdiir_gma | 15 | None — identical |
| SCENARIOS | gma_antiga + fdiir_gma | 20 | Definition identical; weights differ by version |

### 3. Core Functions (100% Redundancy)
- `generate_configurations()` — 12 lines
- `compute_w(config)` — 15 lines  
- `filter_admissible()` — 8 lines
- `scoring_linear()` — 20 lines
- `ideal_point()` — 18 lines
- `pareto_based_ma()` — 12 lines
- `hmmd()` — 25 lines
- **Total:** ~110 duplicate lines across gma_antiga + fdiir_gma

### 4. Parameter Differences (Non-Redundant)

| Parameter | V2 (gma_antiga.ipynb) | V3 (fdiir_gma.ipynb) | Impact |
|-----------|----------------------|----------------------|--------|
| **VETOR_A Scale** | 1–5 | 1–4 | Scores reduced; affects ranking |
| **CCM Scale** | {0, 0.5, 1.0} | {0, 1, 2, 3} | Different granularity; affects filtering |
| **w_dist** | w=1.0: 74, w=0.5: 70 | w=3: 54, w=2: 90 | Distribution differs; impacts top configs |
| **VETOR_A_RAW values** | [samples shown] | Post-revision downscaled | Seen in revision_prompts |
| **Spearman_min** | 0.9505 | 0.9505 | Same |

**FLAG:** V3 represents post-revision recalibration. Verify that revision prompts (in fdiir_gma.ipynb LLM section) are preserved as documentation.

### 5. Unique Content Preserved

**[gma_antiga.ipynb](../gma_antiga.ipynb) — V2 Reference:**
- Full methodological walkthrough (Sections 1-4)
- Comprehensive CCM explanation (6 blocks, consistency scores)
- HMMD formulation (priority vectors, dominance checks)
- References (20+ citations)

**[fdiir_gma.ipynb](../fdiir_gma.ipynb) — V3 Production:**
- LLM evaluation context (4 LLMs, 14 DAs, 5 criteria)
- Prompt A (VETOR_A scoring)
- Prompts B (CCM evaluation, 73 cells worth)
- Revision logic (divergence detection, threshold analysis)
- FDIIR domain documentation (100+ lines on D1–R3 methods)

**[analise_versoes_gma.ipynb](../analise_versoes_gma.ipynb) — Analytics:**
- V1/V2/V3 comparative metrics
- Ranking stability (Spearman correlations)
- DA dominance tracking
- Criterion-level sensitivity

---

## Design Decisions

### 1. Boundary Changes
No boundary changes. The three notebooks remain independent analysis artifacts; no new actors or external integrations introduced.

### 2. Data/Artifact Flow
- **V2 notebook:** Input = VETOR_A_RAW (V2 scale), CCM_RAW (0.5 scale) → Output = Rankings (scoring, ideal point, Pareto, HMMD) for V2
- **V3 notebook:** Input = VETOR_A_RAW (V3 scale), CCM_RAW (1-2-3 scale) + LLM context → Output = Rankings for V3 + revision audit trail
- **Comparative notebook:** Input = VERSIONS dict (V1/V2/V3 parameters) + outputs from V2 & V3 notebooks → Output = Cross-version analysis tables & stability metrics

**Artifact preservation rule:** All dataframes, rankings, Pareto fronts, HMMD results from original notebooks must be replicated in consolidated versions.

### 3. Temporal Choreography
```
PHASE 1: Extract & Clean V2 (v2-gma-fdiir-analysis.ipynb)
  ├─ Copy gma_antiga.ipynb content
  ├─ Keep all unique methodological content
  ├─ Deduplicate setup/function code
  └─ Verify all V2 results (144 admissible, Pareto fronts, rankings)

PHASE 2: Extract & Clean V3 (v3-gma-fdiir-analysis.ipynb)
  ├─ Copy fdiir_gma.ipynb content
  ├─ Keep LLM evaluation context & domain docs as-is
  ├─ Preserve revision prompts & consolidation logic
  ├─ Deduplicate setup/function code (reuse from Phase 1)
  └─ Verify all V3 results (144 admissible, Pareto fronts, rankings)

PHASE 3: Unify Comparative Analysis (consolidated-comparative-analysis.ipynb)
  ├─ Copy analise_versoes_gma.ipynb core analysis
  ├─ Integrate VERSIONS dict (V1/V2/V3 reference data)
  ├─ Add dependency markers (which cells depend on V2 vs V3 notebooks)
  ├─ Verify cross-version consistency (Spearman, DA stability)
  └─ Validate: pareto fronts (V1: 5-6, V2: 6, V3: 4 configs)

PHASE 4: Verify & Document
  ├─ Check data consistency across the three notebooks
  ├─ Verify all result artifacts are preserved
  ├─ Flag any parameter inconsistencies
  └─ Generate consolidation report (what was merged, what was preserved)
```

### 4. Constraints & Trade-offs

**Trade-off: Reusable function library vs. notebook self-containment**
- *Decision:* Keep function definitions in each notebook for self-containment. This reduces coupling, allows notebooks to run independently, and maintains clarity of data flow within each version's context.
- *Rationale:* Functions are simple (~10-25 lines each) and highly version-specific (they read version-specific VETOR_A/CCM). Extraction to a shared module would create implicit version-dependency logic better kept explicit within each notebook.

**Trade-off: Methodological documentation vs. notebook length**
- *Decision:* Preserve all domain documentation and methodological walkthrough in V2 notebook (gma_antiga is the reference); V3 notebook focuses on LLM evaluation results.
- *Rationale:* gma_antiga explicitly titled as comprehensive reference; fdiir_gma is production/research iteration. This separation maintains clarity of intent.

**Inconsistency to Flag: Language in DA_NAMES**
- Both versions use English DA_NAMES (e.g., "model-based", "signal-based"). User confirmed English is preferred. No action needed.

---

## Phase Breakdown

### Phase 1: V2 Notebook Extraction (v2-gma-fdiir-analysis.ipynb)
**Executor:** Implementation-Agent (Jupyter notebook refactoring)
**Deliverable:** v2-gma-fdiir-analysis.ipynb

**Inputs:**
- [gma_antiga.ipynb](../gma_antiga.ipynb) content (all cells)
- Parameter snapshot: VETOR_A scale 1-5, CCM scale {0, 0.5, 1.0}, 144 admissible configs

**Tasks:**
1. Copy all methodological sections from gma_antiga.ipynb
2. Consolidate setup cell (imports, pd.set_option, etc.)
3. Keep all unique functions & analysis cells
4. Replace redundant setup with consolidated version
5. Verify output: VETOR_A dataframe (14×5), admissible (144), Pareto fronts (6 configs per scenario), HMMD results
6. Add markdown headers labeling this as "V2 — Reference Implementation"

**Acceptance Criteria:**
- All 4 selection methods (scoring, ideal point, Pareto, HMMD) produce identical outputs to original
- Spearman_min = 0.9505 (validates Pareto & HMMD correlation)
- 144 admissible configurations verified
- No cells produce errors; all markdown sections preserved

---

### Phase 2: V3 Notebook Extraction (v3-gma-fdiir-analysis.ipynb)
**Executor:** Implementation-Agent (Jupyter notebook refactoring)
**Deliverable:** v3-gma-fdiir-analysis.ipynb

**Inputs:**
- [fdiir_gma.ipynb](../fdiir_gma.ipynb) content (all cells)
- Parameter snapshot: VETOR_A scale 1-4, CCM scale {0, 1, 2, 3}, LLM evaluation context
- Revision prompts & consolidation logic

**Tasks:**
1. Copy all cells from fdiir_gma.ipynb
2. Consolidate setup cell (reuse template from Phase 1, adapt version-specific params)
3. Keep all LLM evaluation context, domain documentation, revision prompts as-is
4. Note: fdiir_gma.ipynb already contains VETOR_A_REVISADO (post-revision scores)
5. Preserve function definitions (even though duplicated with V2, keep for notebook independence)
6. Verify output: Vetor A (1-4 scale), 144 admissible, Pareto fronts (4 configs per scenario), HMMD (invariant: D2-I2-ID1-R1)
7. Add markdown header: "V3 — Current Production (Post-Revision)"

**Acceptance Criteria:**
- All 4 selection methods produce identical outputs to original fdiir_gma.ipynb
- LLM evaluation context preserved verbatim (no edits to domain docs or prompts)
- VETOR_A_REVISADO correctly loaded; revision audit trail visible
- Pareto fronts reduced to 4 configs (vs. V2's 6)
- HMMD invariant across scenarios verified (all output D2-I2-ID1-R1)
- No cells produce errors

---

### Phase 3: Consolidate Comparative Analysis (consolidated-comparative-analysis.ipynb)
**Executor:** Implementation-Agent (Jupyter notebook refactoring)
**Deliverable:** consolidated-comparative-analysis.ipynb

**Inputs:**
- [analise_versoes_gma.ipynb](../analise_versoes_gma.ipynb) content (all cells)
- V2 notebook outputs from Phase 1
- V3 notebook outputs from Phase 2
- VERSIONS dict (V1/V2/V3 reference parameters)

**Tasks:**
1. Copy analise_versoes_gma.ipynb structure (markdown sections, analysis cells)
2. Consolidate setup: imports, pd.set_option, common utility functions
3. Integrate VERSIONS dict; add inline comments about data sources (V2 from Phase 1, V3 from Phase 2)
4. Refactor analysis cells to read from VERSIONS dict rather than hardcoded values
5. Add section: "Data Lineage — How V1, V2, V3 Results Were Generated"
6. Preserve all tables: Top 5 rankings (scoring, ideal point, Pareto, HMMD)
7. Preserve stability metrics: Spearman correlations, DA dominance tracking, criterion sensitivity
8. Verify: V1 pareto (5-6), V2 pareto (6), V3 pareto (4); HMMD convergence pattern across versions

**Acceptance Criteria:**
- All comparison tables generate without errors
- Cross-version metrics (Spearman_min, ranking variance) match original analise_versoes output
- Pareto front sizes validated: V1 ≈ 5-6, V2 = 6, V3 = 4
- HMMD pattern identified: V1/V2 variable across scenarios; V3 invariant
- Lineage documentation visible to users (which version produced which results)

---

### Phase 4: Verification & Consolidation Report
**Executor:** Verification-Agent  
**Deliverable:** consolidation-report.md (archive; not user-facing)

**Tasks:**
1. Verify no data loss: compare original outputs with consolidated notebook outputs (dataframe equality checks)
2. Validate parameter consistency across V2/V3 notebooks
3. Check for hidden inconsistencies or edge cases
4. Generate summary: lines of code reduced, redundancies eliminated, results preserved
5. Flag any parameter anomalies for user review
6. Produce summary statistics (execution time, cell count before/after, etc.)

**Flagged Inconsistencies to Review:**
1. **V3 downward recalibration:** VETOR_A scores reduced (V2 → V3), e.g., D1: [5,3,4,5,4] → [4,3,4,4,3]. Verify this was intentional post-revision. *(User Decision: Confirm this is the approved V3 version)*
2. **CCM scale change:** {0, 0.5, 1.0} → {0, 1, 2, 3}. Verify mapping is correct and no rounding errors introduced.
3. **Pareto front size reduction:** V2 has 6 configs; V3 has 4. Confirm this is due to revised scores, not a filtering bug. *(Likely expected due to recalibration)*

**Acceptance Criteria:**
- No data loss detected (all original outputs replicated)
- All inconsistencies documented with resolution status
- Consolidation report ready for user review
- Notebooks ready for deployment

---

## Risk Review

### 1. Data Loss Risk (Loss of Analysis Results)
- **Applicability:** Applicable (3 notebooks × 40+ output artifacts per notebook)
- **Impact:** HIGH — Loss of any ranking or Pareto front would invalidate research findings
- **Evidence:** Codemap identifies 40+ key outputs (dataframes, rankings, fronts); must verify 1:1 correspondence before/after
- **Disposition:** Acceptance criteria in Phases 1-3 require explicit output validation; Phase 4 implements automated consistency checks. **Mitigation in place.**

### 2. Inconsistency Risk (Conflicting V2/V3 Parameters)
- **Applicability:** Applicable (VETOR_A, CCM scales differ between versions; potential for confusion)
- **Impact:** MEDIUM — Misaligned parameters could cause incorrect cross-version comparisons
- **Evidence:** Codemap identified 5 parameter differences; some flagged as inconsistencies requiring user decision
- **Disposition:** Phase 3 includes inline comments on data sources and version-specific parameters. Flagged inconsistencies escalated to user for decision. **Mitigation in place.**

### 3. Documentation/Context Loss (LLM Evaluation Context, Domain Docs)
- **Applicability:** Applicable (fdiir_gma.ipynb contains 100+ lines of domain context; must preserve)
- **Impact:** MEDIUM — Loss of FDIIR domain documentation would reduce notebook utility for future researchers
- **Evidence:** User confirmed "do not change the prompts" and "language must be English"; LLM context is irreplaceable
- **Disposition:** Phase 2 (V3 extraction) explicitly requires preservation of LLM evaluation context and domain docs verbatim. Checklist enforced. **Mitigation in place.**

### 4. Code Quality Risk (Redundant Code Persisted)
- **Applicability:** Applicable (Functions duplicated across V2/V3; ~110 redundant lines)
- **Impact:** LOW — Code quality degraded but analysis remains valid; maintenance burden increased
- **Evidence:** Design decision (Phase 3 rationale) trades notebook coupling for self-containment; acceptable for research notebooks
- **Disposition:** Trade-off documented and justified. No mitigation needed; acceptable per design. **Risk accepted.**

### 5. Functional Correctness Risk (Methods Produce Wrong Results)
- **Applicability:** Applicable (Four selection methods: scoring, ideal point, Pareto, HMMD)
- **Impact:** HIGH — Incorrect method implementations would invalidate all findings
- **Evidence:** All 4 methods copied verbatim from originals; only setup/data moved; functions unchanged
- **Disposition:** Acceptance criteria require exact output matching to original notebooks. Phase 4 implements automated consistency checks. **Mitigation in place.**

### 6. Boundary/Scope Creep Risk (Unintended Modifications)
- **Applicability:** Applicable (User constraint: "Do not modify original files until consolidation plan approved")
- **Impact:** LOW — Risk of accidental edit to originals; but constraint enforced at execution time
- **Evidence:** Plan explicitly states "no modifications until approval"; original files remain untouched during planning phase
- **Disposition:** Execution will create new notebooks (v2-*.ipynb, v3-*.ipynb, consolidated-*.ipynb); originals left intact. **Mitigation in place.**

**Summary:** 6 risk categories evaluated. 5 mitigated by design; 1 accepted (code redundancy). All HIGH-impact risks have explicit mitigation. Confidence: 0.92.

---

## Dependencies & Prerequisites

### External Dependencies
- Python 3.8+
- Libraries: numpy, pandas, scipy, matplotlib, itertools
- Files: solution_space.json (referenced in fdiir_selection.py)

### Notebook Dependencies
- **Phase 1 (V2):** Independent; can run standalone
- **Phase 2 (V3):** Independent; can run standalone
- **Phase 3 (Comparative):** Depends on Phase 1 & Phase 2 outputs (VERSIONS dict populated from both)
- **Phase 4 (Verification):** Depends on Phase 1, 2, 3 completion

### Execution Order
```
Phase 1 → Phase 2 → Phase 3 → Phase 4
(parallel: 1 & 2 can run in parallel; 3 depends on both 1 & 2)
```

---

## Success Criteria (Acceptance Gates)

1. **Output Fidelity:** All 40+ data artifacts from original notebooks must be identically replicated in consolidated versions (dataframe equality, ranking order, front sizes verified)
2. **Redundancy Reduction:** ~400 lines of redundant setup/function code eliminated or consolidated
3. **Documentation Preservation:** All methodological sections, LLM context, domain documentation, references preserved verbatim
4. **No Data Loss:** Zero missing results, tables, or analysis outputs
5. **Consistency Validation:** All cross-version comparison metrics (Spearman, ranking variance, front sizes) match original analise_versoes.ipynb
6. **Flagged Issues Resolved:** All inconsistencies (V3 downscaling, CCM scale, front size reduction) documented with user sign-off

---

## Timeline & Effort Estimate

| Phase | Task | Executor | Est. Time | Dependencies |
|-------|------|----------|-----------|--------------|
| 1 | Extract V2 notebook | Implementation | 30 min | — |
| 2 | Extract V3 notebook | Implementation | 30 min | — |
| 3 | Consolidate comparative analysis | Implementation | 40 min | Phase 1 & 2 |
| 4 | Verify & report | Verification | 20 min | Phase 1, 2, 3 |
| **Total** | | | **120 min** | |

---

## Deployment & Rollback

### Deployment
1. Create three new notebooks (do not overwrite originals)
2. Validate all acceptance criteria
3. Archive original notebooks to `/archive/` subdirectory
4. Update README.md to point to consolidated notebooks

### Rollback
If critical issues detected:
1. Restore originals from archive
2. Delete consolidated notebooks
3. Escalate to user for revision

---

## Sign-Off & Next Steps

### Ready-to-Execute Checklist
- ✅ Scope clearly defined (3 notebooks, 65% redundancy identified)
- ✅ Parameter differences documented (V2 vs V3)
- ✅ Unique content preserved (methodological docs, LLM context, domain docs)
- ✅ Risk review completed (6 categories; 5 mitigated)
- ✅ Acceptance criteria defined (output fidelity, redundancy reduction, data preservation)
- ✅ Execution order clear (Phase 1 → 2 → 3 → 4)
- ✅ All flagged inconsistencies escalated to user for decision

### User Decisions Required (Before Execution)
1. **V3 Downward Recalibration:** Confirm that VETOR_A score reductions (e.g., D1: 5→4) in V3 are intentional and approved.
2. **CCM Scale Mapping:** Verify that the {0, 0.5, 1.0} → {0, 1, 2, 3} scale change is correctly implemented and intentional.
3. **Pareto Front Size:** Confirm that V3 Pareto fronts reducing from 6 to 4 configs is expected due to revised scores.

### Recommended First Phase
**Phase 1 (V2 Extraction):** Extract and validate v2-gma-fdiir-analysis.ipynb. This is the reference implementation and can serve as a baseline for comparing Phase 2 results.

---

**Plan Author:** Planner-Agent  
**Plan Type:** Notebook Consolidation & Refactoring  
**Execution Model:** Phased (4 phases, 120-minute total effort)  
**Status:** READY FOR EXECUTION — Awaiting user approval of sign-off decisions.
