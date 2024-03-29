using LinearAlgebra
include("viz.jl"); import .Viz

function euler(G, dt, x, t)
    dxdt = G(x, t)
    return x + dxdt * dt
end

function heun(G, dt, x, t)
    v = G(x, t)
    w = G(x+v*dt, t+dt)
    return x + (v + w) * dt / 2
end

function RK4th(G, dt, x, t)
    v_1 = G(x, t)
    v_2 = G(x + v_1 * dt / 2, t + dt / 2)
    v_3 = G(x + v_2 * dt / 2, t + dt / 2)
    v_4 = G(x + v_3 * dt, t + dt)
    v = (v_1 + 2v_2 + 2v_3 + v_4) / 6
    return x + v * dt
end

# 荷電粒子の運動
function prob1(advance;
        q=1., m=1., T=4π, N=100,
        E=((x, t) -> [0., 0., 0.]),
        B=((x, t) -> [0., 0., 1.]),
        x0=[0., 0., 0.], v0=[1., 0., 0.])

    function G(f, t)
        x = f[1:3]
        v = f[4:6]
        a = q/m * (E(x, t) + v × B(x, t))
        return [v; a]
    end

    dt = T / N
    f = [x0; v0]

    Channel{typeof(f)}(32) do channel
        put!(channel, f)
        for i = 1:N
            f = advance(G, dt, f, i*dt)
            put!(channel, f)
        end
    end
end

function sample1(;method=euler, N=200)
    gen = prob1(method; N)
    Viz.plot(gen, N+1)
end

# ケプラー問題
function prob2(advance; e=0.5, period=1, N=200)
    T = period * 2π
    function G(u, _)
        r = u[1:2]
        v = u[3:4]
        R⁻³ = (r' * r)^(-3/2)
        return [v; -r*R⁻³]
    end

    dt = T / N
    println("dt = ", dt)
    u = [1-e, 0, 0, sqrt((1-e)/(1+e))] # (x, y, vx, vy)

    Channel{typeof(u)}(32) do channel
        put!(channel, u)
        for i = 1:N
            u = advance(G, dt, u, i*dt)
            put!(channel, u)
        end
    end
end

function sample2(;e=0.6, period=1, method=RK4th, N=100000)
    gen = prob2(method; e, period, N)
    plt = Viz.decimated_plot(gen, N+1)
    plot!(plt, title="e=$(e), period=$(period), N=$(N)")
    plt
end

function prob3(advance; e=0.5, period=1, ds=0.1)
    T = period * 2π
    function G(u, _)
        r = u[1:2]
        v = u[3:4]
        R² = r' * r
        R⁻¹ = R²^(-1/2)
        return [v; v*(r' * v)/R² - r*R⁻¹]
    end

    u = [1-e, 0, 0, (1-e)*sqrt((1-e)/(1+e))]

    Channel{typeof(u)}(32) do channel
        put!(channel, u)
        t_total = 0
        i = 0
        while t_total < T
            u = advance(G, ds, u, i*ds)
            put!(channel, u)
            R = norm(u[1:2])
            t_total += R * ds
            i += 1
        end
    end
end

function sample3(;e=0.6, period=1, method=RK4th, ds=0.1)
    gen = prob3(method; e, period, ds)
    plt = Viz.plot_all(gen)
    plt
end

if abspath(PROGRAM_FILE) == @__FILE__
    display(sample1())
end