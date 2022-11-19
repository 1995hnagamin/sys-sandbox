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

teff(tm, (t1, t2, tx)) = isvalid(tx) ? eff(tm, (1, tx)) : eff(tm, (t1, t2))

function maxeff((s1, s2, sx), (t1, t2, tx))
    tty = isvalid(tx) ? (1, tx) : (t1, t2)
    multiplier(s) = eff(s, tty) * (s == sx ? 2 : 1.5)

    max(multiplier(s1), multiplier(s2), eff(sx, tty)*1.5)
end


types = [
    "---",
    "WTR", "ELC", "GRD", "FLY"
]
N_TYPES = length(types)


ctyidx = Dict{Tuple{String, String, String}, Int}()
ctyrev = Dict{Int, Tuple{String, String, String}}()
for (i, ((t1, t2), tx)) in enumerate(IterTools.product(IterTools.subsets(types, Val{2}()), types))
    ctyidx[t1, t2, tx] = i
    ctyrev[i] = (t1, t2, tx)
end

sz = N_TYPES * (N_TYPES-1) ÷ 2 * N_TYPES
T = ones(sz, sz) / sz

scores = ones(1, sz) / sz
cezarom = ones(1, sz) / sz
for iter = 1:1000
    global scores, cezarom
    expect = Dict{Tuple{String, String, String},Float64}()
    for (t1, t2) in IterTools.subsets(types, Val{2}())
        for s in types
            ptot = scores[ctyidx[t1, t2, "---"]]
            ex = eff(tyidx[s], (tyidx[t1], tyidx[t2]))
            for tx in types
                p = scores[ctyidx[t1, t2, tx]]
                ptot += p
                ex += p * eff(tyidx[s], (tyidx[tx], 1))
            end
            ex /= ptot
            expect[s, t1, t2] = ex
        end
    end
    function select((s1, s2, sx), (t1, t2))
        stab(s) = (s == sx ? 2 : 1.5)
        cands = sort([
            (stab(s1)*expect[s1, t1, t2], s1),
            (stab(s2)*expect[s2, t1, t2], s2),
            (1.5*expect[sx, t1, t2], sx)
        ], rev=true)
        return cands[1][2]
    end
    A = zeros(sz, sz)
    for j = 1:sz
        (t1, t2, tx) = ctyrev[j]
        for i = 1:sz
            (s1, s2, sx) = ctyrev[i]
            tty = (tyidx[t1], tyidx[t2], tyidx[tx])
            sty = (tyidx[s1], tyidx[s2], tyidx[sx])
            wst = max(1.5/8, teff(tyidx[select((s1, s2, sx), (t1, t2))], tty))
            wts = max(1.5/8, teff(tyidx[select((t1, t2, tx), (s1, s2))], sty))
            weight = log(2, wts/wst)
            if weight > 0
                A[i, j] = weight
            end
        end
    end
    S = zeros(sz, sz)
    for i = 1:sz
        S[i, :] = A[i, :]/sum(A[i,:])
    end
    G = 0.85*S + 0.15*T
    new_scores = scores * G
    new_cezarom = cezarom * iter / (iter + 1) + new_scores / (iter + 1)
    diff = LinearAlgebra.norm1(new_cezarom - cezarom)/sz
    println(diff)
    scores = new_scores
    cezarom = new_cezarom
    #=
    result = sort([(scores[i], ctyrev[i]) for i in 1:sz], rev=true)
    @printf("[Top 5]\n")
    for i = 1:5
        score, (t1, t2, tx) = result[i]
        if t1 == "---"
            t1, t2 = t2, t1
        end
        @printf("%02d: %s / %s (%s) ... %.10f\n", i, t1, t2, tx, score)
    end
    =#
end

rank = cezarom / maximum(cezarom) * 100
result = sort([(rank[i], ctyrev[i]) for i in 1:sz], rev=true)

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
        rank[ctyidx[t1, t2, tx]]
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
