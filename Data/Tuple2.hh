<?hh // strict

namespace Data
{

    final class Tuple2<+Tt, +Tu>
    {
        final private function __construct(private Tt $first, private Tu $second)
        {
        }

        final public static function make(Tt $x, Tu $y): Tuple2<Tt, Tu>
        {
            return new self($x, $y);
        }

        final public function match<Tv>((function (Tt, Tu): Tv) $f): Tv
        {
            return $f($this->first, $this->second);
        }

        final public function first(): Tt
        {
            return $this->match(($x, $y) ==> $x);
        }

        final public function second(): Tu
        {
            return $this->match(($x, $y) ==> $y);
        }

        final public function toTuple(): (Tt, Tu)
        {
            return $this->match(($x, $y) ==> tuple($x, $y));
        }

        final public static function fromTuple((Tt, Tu) $x): Tuple2<Tt, Tu>
        {
            return Tuple2::make($x[0], $x[1]);
        }

        final public function toPair(): Pair<Tt, Tu>
        {
            return $this->match(($x, $y) ==> Pair { $x, $y });
        }

        final public static function fromPair(Pair<Tt, Tu> $x): Tuple2<Tt, Tu>
        {
            return Tuple2::make($x[0], $x[1]);
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Tuple2<Tt, Tv>
        {
            return $this->bimap($x ==> $x, $f);
        }

        final public function bimap<Tv, Tw>((function (Tt): Tv) $f, (function (Tu): Tw) $g): Tuple2<Tv, Tw>
        {
            return $this->match(($x, $y) ==> Tuple2::make($f($x), $g($y)));
        }
    }

}

