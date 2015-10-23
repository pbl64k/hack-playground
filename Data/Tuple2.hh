<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IFunctor.hh');
    require_once(__DIR__.'/IBifunctor.hh');

    final class Tuple2Prx
    {
        final private function __construct()
        {
        }
    }

    final class Tuple2<Tp as Tuple2Prx, +Tt, +Tu> implements IFunctor<Tp, Tu>, IBifunctor<Tp, Tt, Tu>
    {
        final private function __construct(private Tt $first, private Tu $second)
        {
        }

        final public static function make(Tt $x, Tu $y): Tuple2<Tp, Tt, Tu>
        {
            return new self($x, $y);
        }

        final public function match<Tv>((function (Tt, Tu): Tv) $f): Tv
        {
            return $f($this->first, $this->second);
        }

        final public function asFunctor<Tv>((function (IFunctor<Tp, Tu>): IFunctor<Tp, Tv>) $f): Tuple2<Tp, Tt, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Tuple2, 'unique proxy type');
            return $x;
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Tuple2<Tp, Tt, Tv>
        {
            return $this->bimap($x ==> $x, $f);
        }

        final public function asBifunctor<Tv, Tw>((function (IBifunctor<Tp, Tt, Tu>): IFunctor<Tv, Tw>) $f): Tuple2<Tp, Tv, Tw>
        {
            $x = $f($this);
            invariant($x instanceof Tuple2, 'unique proxy type');
            return $x;
        }

        final public function bimap<Tv, Tw>((function (Tt): Tv) $f, (function (Tu): Tw) $g): Tuple2<Tp, Tv, Tw>
        {
            return $this->match(($x, $y) ==> Tuple2::make($f($x), $g($y)));
        }

        final public function first(): Tt
        {
            return $this->match(($x, $y) ==> $x);
        }

        final public function second(): Tu
        {
            return $this->match(($x, $y) ==> $y);
        }

        final public static function fromTuple((Tt, Tu) $x): Tuple2<Tp, Tt, Tu>
        {
            return Tuple2::make($x[0], $x[1]);
        }

        final public function toTuple(): (Tt, Tu)
        {
            return $this->match(($x, $y) ==> tuple($x, $y));
        }

        final public static function fromPair(Pair<Tt, Tu> $x): Tuple2<Tp, Tt, Tu>
        {
            return Tuple2::make($x[0], $x[1]);
        }

        final public function toPair(): Pair<Tt, Tu>
        {
            return $this->match(($x, $y) ==> Pair { $x, $y });
        }
    }

    type Tuple2T<+Tt, +Tu> = Tuple2<Tuple2Prx, Tt, Tu>;

}

