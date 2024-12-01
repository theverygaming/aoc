import System.IO
import Control.Monad
import Data.List (sort)

main = do
    let listfull = []
    -- file reading: https://stackoverflow.com/a/7867786
    handle <- openFile "input.txt" ReadMode
    contents <- hGetContents handle
    let singlewords = words contents
        listfull = map read singlewords :: [Int]
    --print listfull
    -- splitting the list https://www.reddit.com/r/haskell/comments/1cj8lc/comment/c9h2vxt
    -- filtering list of tuples https://stackoverflow.com/a/1618477
    let listleft = map (fst) (filter (even.snd) (zip listfull [0..]))
    let listright = map (fst) (filter (odd.snd) (zip listfull [0..]))
    --print listleft
    --print listright
    print (sum (map (\(a, b) -> abs (a - b)) (zip (sort listleft) (sort listright))))
    print (sum (map (\a -> a * (count listright a)) listleft))
    hClose handle

count :: [Int] -> Int -> Int
count x val = length (filter (== val) x)
