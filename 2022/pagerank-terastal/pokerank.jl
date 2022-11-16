using LinearAlgebra
using Printf
import IterTools

#=
    NRM: ノーマル
    FIR: ほのお
    WTR: みず
    ELC: でんき
    GRS: くさ
    ICE: こおり
    FGT: かくとう
    PSN: どく
    GRD: じめん
    FLY: ひこう
    PSY: エスパー
    BUG: むし
    RCK: いわ
    GHT: ゴースト
    DRG: ドラゴン
    DRK: あく
    STL: はがね
    FRY: フェアリー
=#
types = [
    "---",
    "NRM", "FIR", "WTR", "ELC", "GRS",
    "ICE", "FGT", "PSN", "GRD", "FLY",
    "PSY", "BUG", "RCK", "GHT", "DRG",
    "DRK", "STL", "FRY"
]
N_TYPES = length(types)

tyidx = Dict{String, Int}()
for (i, type) in enumerate(types)
    tyidx[type] = i
end

M = [
    0    0  0  0  0  0    0  0  0  0  0    0  0  0  0  0    0  0  0;

    1    1  1  1  1  1    1  1  1  1  1    1  1 .5  0  1    1 .5  1;
    1    1 .5 .5  1  2    2  1  1  1  1    1  2 .5  1 .5    1  2  1;
    1    1  2 .5  1 .5    1  1  1  2  1    1  1  2  1 .5    1  1  1;
    1    1  1  2 .5 .5    1  1  1  0  2    1  1  1  1 .5    1  1  1;
    1    1 .5  2  1 .5    1  1 .5  2 .5    1 .5  2  1 .5    1 .5  1;

    1    1 .5 .5  1  2   .5  1  1  2  2    1  1  1  1  2    1 .5  1;
    1    2  1  1  1  1    2  1 .5  1 .5   .5 .5  2  0  1    2  2 .5;
    1    1  1  1  1  2    1  1 .5 .5  1    1  1 .5 .5  1    1  0  2;
    1    1  2  1  2 .5    1  1  2  1  0    1 .5  2  1  1    1  2  1;
    1    1  1  1 .5  2    1  2  1  1  1    1  2 .5  1  1    1 .5  1;

    1    1  1  1  1  1    1  2  2  1  1   .5  1  1  1  1    0 .5  1;
    1    1 .5  1  1  2    1 .5 .5  1 .5    2  1  1 .5  1    2 .5 .5;
    1    1  2  1  1  1    2 .5  1 .5  2    1  2  1  1  1    1 .5  1;
    1    0  1  1  1  1    1  1  1  1  1    2  1  1  2  1   .5  1  1;
    1    1  1  1  1  1    1  1  1  1  1    1  1  1  1  2    1 .5  0;

    1    1  1  1  1  1    1 .5  1  1  1    2  1  1  2  1   .5  1 .5;
    1    1 .5 .5 .5  1    2  1  1  1  1    1  1  2  1  1    1 .5  2;
    1    1 .5  1  1  1    1  2 .5  1  1    1  1  1  1  2    2 .5  1
]

# effectiveness of a move of type tm
eff(tm, (ty1, ty2)) = M[tm, ty1] * M[tm, ty2]

isvalid(ty) = (ty != 1)

function maxeff((s1, s2, sx), (t1, t2, tx))
    tty = isvalid(tx) ? (tx, 1) : (t1, t2)
    multiplier(s) = eff(s, tty) * (s == sx ? 2 : 1.5)

    max(multiplier(s1), multiplier(s2), eff(sx, tty)*1.5)
end

#=
types = [
    "---",
    "FIR", "WTR", "ELC", "GRD"
]
N_TYPES = length(types)
=#

ctyidx = Dict{Tuple{String, String, String}, Int}()
ctrev = Dict{Int, Tuple{String, String, String}}()
for (i, ((t1, t2), tx)) in enumerate(IterTools.product(IterTools.subsets(types, Val{2}()), types))
    ctyidx[t1, t2, tx] = i
    ctrev[i] = (t1, t2, tx)
end

sz = N_TYPES * (N_TYPES-1) ÷ 2 * N_TYPES
A = zeros(sz, sz)
for ((t1, t2), tx) in IterTools.product(IterTools.subsets(types, Val{2}()), types)
    j = ctyidx[t1, t2, tx]
    tty = (tyidx[t1], tyidx[t2], tyidx[tx])
    for ((s1, s2), sx) in IterTools.product(IterTools.subsets(types, Val{2}()), types)
        i = ctyidx[s1, s2, sx]
        sty = (tyidx[s1], tyidx[s2], tyidx[sx])
        weight = log(2, max(maxeff(tty, sty),1.5/8)/max(maxeff(sty, tty),1.5/8))
        if weight > 0
            A[i,j] = weight
        end
    end
end

S = zeros(sz, sz)
for i = 1:sz
    S[i, :] = A[i, :]/sum(A[i,:])
end

T = ones(sz, sz) / sz
G = 0.85*transpose(S) + 0.15*T

F = eigen(G)
scores = Vector{Float64}(F.vectors[:,sz])
if scores[1] < 0
    scores *= -1
end
scores = scores / maximum(scores) * 100
result = sort([(scores[i], ctrev[i]) for i in 1:sz], rev=true)


@printf("[Top 30]\n")
for i = 1:30
    score, (t1, t2, tx) = result[i]
    if t1 == "---"
        t1, t2 = t2, t1
    end
    @printf("%02d: %s / %s (%s) ... %.10f\n", i, t1, t2, tx, score)
end

open("pokerank.csv", "w") do csv
    @printf(csv, "# Type 1, Type 2, Tera Type, Score\n")
    for row in result
        score, (t1, t2, tx) = row
        if t1 == "---"
            t1, t2 = t2, t1
        end
        @printf(csv, "%s,%s,%s,%.10f\n", t1, t2, tx, score)
    end
end


average(xs) = sum(xs)/length(xs)
ttas = sort([
    (average([scores[ctyidx[t1, t2, tx]] for (t1, t2) in IterTools.subsets(types, Val{2}())]), tx)
    for tx in types
], rev=true)

@printf("\n[Tera Type average score]\n")
for (score, ty) in sort(ttas,rev=true)
    @printf("%s: %.10f\n", ty, score)
end

open("ttas.csv", "w") do csv
    @printf(csv, "# Tera Type, Average score\n")
    for row in ttas
        score, tx = row
        @printf(csv, "%s,%.10f\n", tx, score)
    end
end
