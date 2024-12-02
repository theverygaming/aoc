import System.IO
import Control.Monad
import Data.List (sort)

main = do
    let input = []
    -- file reading: https://stackoverflow.com/a/7867786
    handle <- openFile "input.txt" ReadMode
    contents <- hGetContents handle
    let inputlines = map words (lines contents)
        input = map (\x -> map read x) inputlines :: [[Int]]
    --print inputlines
    --print input
    print (length (filter id (map (\x -> isvalid (abslist (map (\(a:b:[]) -> a - b) (windows 2 (x))))) input)))
    hClose handle

isvalid :: [Int] -> Bool
isvalid x = (length x) == (length (filter (\y -> (y >= 1 && y <= 3)) x))

abslist xz@(x:xs) = if x > 0 then xz else (map negate xz)

-- https://stackoverflow.com/q/27726739
-- note i still didn't quite understand this function :sob: but it's 11pm
windows n xz@(x:xs) -- `xz@(x:xs)` decomposes the list xz into the first element `x` and the other elements `xs` 
  | length v < n = []
  | otherwise = v : windows n xs
  where
    v = take n xz
