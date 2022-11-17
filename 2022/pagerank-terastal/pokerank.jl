import LinearAlgebra
using Printf
import IterTools
import Statistics

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
    #    N  F  W  E  G    I  F  P  G  F    P  B  R  G  D    D  S  F
    #    R  I  T  L  R    C  G  S  R  L    S  U  C  H  R    R  T  R
    #    M  R  R  C  S    E  T  N  D  Y    Y  G  K  T  G    K  L  Y

    0    0  0  0  0  0    0  0  0  0  0    0  0  0  0  0    0  0  0;

    1    1  1  1  1  1    1  1  1  1  1    1  1 .5  0  1    1 .5  1;    # NRM
    1    1 .5 .5  1  2    2  1  1  1  1    1  2 .5  1 .5    1  2  1;    # FIR
    1    1  2 .5  1 .5    1  1  1  2  1    1  1  2  1 .5    1  1  1;    # WTR
    1    1  1  2 .5 .5    1  1  1  0  2    1  1  1  1 .5    1  1  1;    # ELC
    1    1 .5  2  1 .5    1  1 .5  2 .5    1 .5  2  1 .5    1 .5  1;    # GRS

    1    1 .5 .5  1  2   .5  1  1  2  2    1  1  1  1  2    1 .5  1;    # ICE
    1    2  1  1  1  1    2  1 .5  1 .5   .5 .5  2  0  1    2  2 .5;    # FGT
    1    1  1  1  1  2    1  1 .5 .5  1    1  1 .5 .5  1    1  0  2;    # PSN
    1    1  2  1  2 .5    1  1  2  1  0    1 .5  2  1  1    1  2  1;    # GRD
    1    1  1  1 .5  2    1  2  1  1  1    1  2 .5  1  1    1 .5  1;    # FLY

    1    1  1  1  1  1    1  2  2  1  1   .5  1  1  1  1    0 .5  1;    # PSY
    1    1 .5  1  1  2    1 .5 .5  1 .5    2  1  1 .5  1    2 .5 .5;    # BUG
    1    1  2  1  1  1    2 .5  1 .5  2    1  2  1  1  1    1 .5  1;    # RCK
    1    0  1  1  1  1    1  1  1  1  1    2  1  1  2  1   .5  1  1;    # GHT
    1    1  1  1  1  1    1  1  1  1  1    1  1  1  1  2    1 .5  0;    # DRG

    1    1  1  1  1  1    1 .5  1  1  1    2  1  1  2  1   .5  1 .5;    # DRK
    1    1 .5 .5 .5  1    2  1  1  1  1    1  1  2  1  1    1 .5  2;    # STL
    1    1 .5  1  1  1    1  2 .5  1  1    1  1  1  1  2    2 .5  1     # FRY
]

# effectiveness of a move of type tm
eff(tm, (ty1, ty2)) = M[tm, ty1] * M[tm, ty2]

isvalid(ty) = (ty != 1)

function maxeff((s1, s2, sx), (t1, t2, tx))
    tty = isvalid(tx) ? (1, tx) : (t1, t2)
    multiplier(s) = eff(s, tty) * (s == sx ? 2 : 1.5)

    max(multiplier(s1), multiplier(s2), eff(sx, tty)*1.5)
end

types = [
    "---",
    "NRM", "FIR", "WTR", "ELC", "GRS",
    "ICE", "FGT", "PSN", "GRD", "FLY",
    "PSY", "BUG", "RCK", "GHT", "DRG",
    "DRK", "FRY"
]
N_TYPES = length(types)

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

F = LinearAlgebra.eigen(G)
scores = Vector{Float64}(F.vectors[:,sz])
if sum(scores) < 0
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

ttscore = Dict{String,Vector{Float64}}()
for tx in types
    ttscore[tx] = [
        scores[ctyidx[t1, t2, tx]]
        for (t1, t2) in IterTools.subsets(types, Val{2}())
    ]
end

ttas = sort([(Statistics.mean(ttscore[tx]), tx) for tx in types], rev=true)

@printf("\n[Tera Type average score]\n")
for (score, ty) in sort(ttas,rev=true)
    @printf("%s: %.10f\n", ty, score)
end

open("ttas.csv", "w") do csv
    @printf(csv, "# Tera Type, mean, std, min, 25%%, 50%%, 75%%, max\n")
    for row in ttas
        mean, tx = row
        std = Statistics.std(ttscore[tx])
        qnt = Statistics.quantile(ttscore[tx], [0.25, 0.5, 0.75])
        minm = minimum(ttscore[tx])
        maxm = maximum(ttscore[tx])
        @printf(csv, "%s,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f\n",
            tx, mean,std,minm,qnt[1],qnt[2],qnt[3],maxm)
    end
end
