<?hh // strict

namespace Data
{

    abstract class Variant<Tt, +Tu>
    {
        final public static function left(Tt $x): Variant<Tt, Tu>
        {
            return Left::make($x);
        }

        final public static function right(Tu $x): Variant<Tt, Tu>
        {
            return Right::make($x);
        }

        abstract public function match<Tv>((function (Tt): Tv) $ifLeft, (function (Tu): Tv) $ifRight): Tv;

        final public function fmap<Tv>((function (Tu): Tv) $f): Variant<Tt, Tv>
        {
            return $this->bimap($x ==> $x, $f);
        }

        final public function bimap<Tv, Tw>((function (Tt): Tv) $f, (function (Tu): Tw) $g): Variant<Tv, Tw>
        {
            return $this->match($x ==> Variant::left($f($x)), $x ==> Variant::right($g($x)));
        }

        final public static function pure(Tu $x): Variant<Tt, Tu>
        {
            return Variant::right($x);
        }

        final public static function flatten(Variant<Tt, Variant<Tt, Tu>> $x): Variant<Tt, Tu>
        {
            return $x->flatMap($x ==> $x);
        }

        final public function flatMap<Tv>((function (Tu): Variant<Tt, Tv>) $f): Variant<Tt, Tv>
        {
            return $this->match($x ==> Variant::left($x), $f);
        }
    }

    final class Left<Tt, +Tu> extends Variant<Tt, Tu>
    {
        final private function __construct(private Tt $x)
        {
        }

        final public static function make(Tt $x): Left<Tt, Tu>
        {
            return new self($x);
        }

        final public function match<Tv>((function (Tt): Tv) $ifLeft, (function (Tu): Tv) $ifRight): Tv
        {
            return $ifLeft($this->x);
        }
    }

    final class Right<Tt, +Tu> extends Variant<Tt, Tu>
    {
        final private function __construct(private Tu $x)
        {
        }

        final public static function make(Tu $x): Right<Tt, Tu>
        {
            return new self($x);
        }

        final public function match<Tv>((function (Tt): Tv) $ifLeft, (function (Tu): Tv) $ifRight): Tv
        {
            return $ifRight($this->x);
        }
    }

}

