<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IMonad.hh');
    require_once(__DIR__.'/IBifunctor.hh');

    final class VariantPrx
    {
        final private function __construct()
        {
        }
    }

    abstract class Variant<Tp as VariantPrx, +Tt, +Tu> implements IMonad<Tp, Tu>, IBifunctor<Tp, Tt, Tu>
    {
        final public static function left(Tt $x): Variant<Tp, Tt, Tu>
        {
            return Left::make($x);
        }

        final public static function right(Tu $x): Variant<Tp, Tt, Tu>
        {
            return Right::make($x);
        }

        abstract public function match<Tv>((function (Tt): Tv) $ifLeft, (function (Tu): Tv) $ifRight): Tv;

        final public function asFunctor<Tv>((function (IFunctor<Tp, Tu>): IFunctor<Tp, Tv>) $f): Variant<Tp, Tt, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Variant, 'unique proxy type');
            return $x;
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Variant<Tp, Tt, Tv>
        {
            return $this->bimap($x ==> $x, $f);
        }

        final public function asBifunctor<Tv, Tw>((function (IBifunctor<Tp, Tt, Tu>): IFunctor<Tv, Tw>) $f): Variant<Tp, Tv, Tw>
        {
            $x = $f($this);
            invariant($x instanceof Variant, 'unique proxy type');
            return $x;
        }

        final public function bimap<Tv, Tw>((function (Tt): Tv) $f, (function (Tu): Tw) $g): Variant<Tp, Tv, Tw>
        {
            return $this->match($x ==> Variant::left($f($x)), $x ==> Variant::right($g($x)));
        }

        final public function asMonad<Tv>((function (IMonad<Tp, Tu>): IMonad<Tp, Tv>) $f): Variant<Tp, Tt, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Variant, 'unique proxy type');
            return $x;
        }

        final public function flatMap<Tv>((function (Tu): IMonad<Tp, Tv>) $f): Variant<Tp, Tt, Tv>
        {
            return $this->match($x ==> Variant::left($x), $x ==>
                    {
                        $z = $f($x);
                        invariant($z instanceof Variant, 'unique proxy type');
                        return $z;
                    });
        }

        final public function singleton<Tv>(Tv $x): Variant<Tp, Tt, Tv>
        {
            return Variant::right($x);
        }

        final public static function flatten(Variant<Tp, Tt, Variant<Tp, Tt, Tu>> $x): Variant<Tp, Tt, Tu>
        {
            return $x->flatMap($x ==> $x);
        }
    }

    final class Left<Tp as VariantPrx, +Tt, +Tu> extends Variant<Tp, Tt, Tu>
    {
        final private function __construct(private Tt $x)
        {
        }

        final public static function make(Tt $x): Left<Tp, Tt, Tu>
        {
            return new self($x);
        }

        final public function match<Tv>((function (Tt): Tv) $ifLeft, (function (Tu): Tv) $ifRight): Tv
        {
            return $ifLeft($this->x);
        }
    }

    final class Right<Tp as VariantPrx, +Tt, +Tu> extends Variant<Tp, Tt, Tu>
    {
        final private function __construct(private Tu $x)
        {
        }

        final public static function make(Tu $x): Right<Tp, Tt, Tu>
        {
            return new self($x);
        }

        final public function match<Tv>((function (Tt): Tv) $ifLeft, (function (Tu): Tv) $ifRight): Tv
        {
            return $ifRight($this->x);
        }
    }

    type VariantT<+Tt, +Tu> = Variant<VariantPrx, Tt, Tu>;

}

