using LinearAlgebra
import IterTools

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

ctyidx = Dict{Tuple{String, String}, Int}()
ctrev = Dict{Int, Tuple{String, String}}()
for (i, (t1, t2)) in enumerate(IterTools.subsets(types, Val{2}()))
    ctyidx[t1, t2] = i
    ctrev[i] = (t1, t2)
end

sz = N_TYPES * (N_TYPES-1) ÷ 2
A = zeros(sz, sz)
for (t1, t2) in IterTools.subsets(types, Val{2}())
    j = ctyidx[t1, t2]
    tty = (tyidx[t1], tyidx[t2], 1)
    for (s1, s2) in IterTools.subsets(types, Val{2}())
        i = ctyidx[s1, s2]
        sty = (tyidx[s1], tyidx[s2], 1)
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

result = sort([(abs(scores[i]), ctrev[i]) for i in 1:sz], rev=true)
