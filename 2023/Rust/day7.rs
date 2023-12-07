use std::fs;
use std::convert::TryFrom;

// from strongest to weakest
#[derive(Debug)]
enum HandType {
    FiveOfAKind,
    FourOfAKind,
    FullHouse,
    ThreeOfAKind,
    TwoPair,
    OnePair,
    HighCard,
    Default,
}

#[derive(Copy, Clone, Debug)]
enum CardType {
    TA = 12,
    TK = 11,
    TQ = 10,
    TJ = 9,
    TT = 8,
    T9 = 7,
    T8 = 6,
    T7 = 5,
    T6 = 4,
    T5 = 3,
    T4 = 2,
    T3 = 1,
    T2 = 0,
    Default = -1,
}

struct Hand {
    cards: [CardType; 5],
    t: HandType,
}

fn getHandType(h: &Hand) -> HandType {
    HandType::FiveOfAKind
}

fn getCardType(c: &char) -> CardType {
    return match c {
        'A' => CardType::TA,
        'K' => CardType::TK,
        'Q' => CardType::TQ,
        'J' => CardType::TJ,
        'T' => CardType::TT,
        '9' => CardType::T9,
        '8' => CardType::T8,
        '7' => CardType::T7,
        '6' => CardType::T6,
        '5' => CardType::T5,
        '4' => CardType::T4,
        '3' => CardType::T3,
        '2' => CardType::T2,
        _ => panic!("invalid card"),
    }
}

fn parseHand(s: &str) -> Hand {
    if s.len() != 5 {
        panic!("invalid len");
    }
    let chars: Vec<char> = s.chars().collect();
    let mut hand = Hand {
        cards: [CardType::Default; 5],
        t: HandType::Default,
    };
    for i in 0..5 {
        hand.cards[i] = getCardType(&chars[i])
    }
    hand.t = getHandType(&hand);
    return hand;
}


fn main() {
    let rawtext = fs::read_to_string("day7_sample.txt").expect("couldn't read file");
    for line in rawtext.lines() {
        let split = line.split(" ").collect::<Vec<&str>>();
        let hand = split[0];
        let bet = split[1].parse::<i32>().unwrap();
        println!("line: hand: {hand} bet: {bet}");
        let h = parseHand(hand);
        println!("    {:?}", h.t);
    }
}
