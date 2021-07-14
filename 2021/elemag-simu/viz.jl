module Viz

export plot, decimated_scatter

import Plots

function plot(gen, N; idx1=1, idx2=2)
    x = zeros(Float32, N)
    y = zeros(Float32, N)
    i = 1
    for u in gen
        x[i] = u[idx1]
        y[i] = u[idx2]
        i += 1
    end
    Plots.plot(x, y, marker=2)
end

function decimated_scatter(gen, N; idx1=1, idx2=2)
    bufsize = Int(floor(sqrt(N)))
    plt = Plots.scatter(1, aspect_ratio=:equal, marker=2, legend=false)
    # plot √N points
    for block in Iterators.partition(gen, bufsize)
        u = popfirst!(block)
        push!(plt, u[idx1], u[idx2])
    end
    plt
end

end