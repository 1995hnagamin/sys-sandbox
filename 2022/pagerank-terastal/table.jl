import CSV
using Printf
import DataFrames

typeja = Dict(
    "---" => "--",
    "NRM" => "ノーマル",
    "FIR" => "ほのお",
    "WTR" => "みず",
    "ELC" => "でんき",
    "GRS" => "くさ",
    "ICE" => "こおり",
    "FGT" => "かくとう",
    "PSN" => "どく",
    "GRD" => "じめん",
    "FLY" => "ひこう",
    "PSY" => "エスパー",
    "BUG" => "むし",
    "RCK" => "いわ",
    "GHT" => "ゴースト",
    "DRG" => "ドラゴン",
    "DRK" => "あく",
    "STL" => "はがね",
    "FRY" => "フェアリー"
)

df = CSV.read("wgef/pokerank.csv", DataFrames.DataFrame)

open("table.txt", "w") do fout
    for (i, row) in enumerate(eachrow(df[1:30,:]))
        @printf(fout,
            "<tr>\n  <td>%d</td>\n  <td>%s・%s</td>\n  <td>%s</td>\n  <td>%.1f</td>\n</tr>",
            i, typeja[row[1]], typeja[row[2]], typeja[row[3]], row[4])
    end
end
