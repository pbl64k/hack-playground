<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IMonad.hh');
    require_once(__DIR__.'/Variant.hh');

    final class LazyPrx
    {
        final private function __construct()
        {
        }
    }

    final class Lazy<Tp as LazyPrx, +Tu> implements IMonad<Tp, Tu>
    {
        final private function __construct(private VariantT<(function (): Tu), Tu> $x)
        {
        }

        final public static function delay((function (): Tu) $f): Lazy<Tp, Tu>
        {
            return new self(Variant::left($f));
        }

        final public static function promote(Tu $x): Lazy<Tp, Tu>
        {
            return new self(Variant::right($x));
        }

        final public function match<Tv>((function (Tu): Tv) $f): Tv
        {
            return $f($this->force());
        }

        final public function asFunctor<Tv>((function (IFunctor<Tp, Tu>): IFunctor<Tp, Tv>) $f): Lazy<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Lazy, 'unique proxy type');
            return $x;
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Lazy<Tp, Tv>
        {
            return Lazy::delay(() ==> $f($this->force()));
        }

        final public function asMonad<Tv>((function (IMonad<Tp, Tu>): IMonad<Tp, Tv>) $f): Lazy<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Lazy, 'unique proxy type');
            return $x;
        }

        final public function flatMap<Tv>((function (Tu): IMonad<Tp, Tv>) $f): Lazy<Tp, Tv>
        {
            return $this->match($x ==>
                    {
                        $z = $f($x);
                        invariant($z instanceof Lazy, 'unique proxy type');
                        return $z;
                    });
        }

        final public function singleton<Tv>(Tv $x): Lazy<Tp, Tv>
        {
            return Lazy::promote($x);
        }

        final public static function flatten(Lazy<Tp, Lazy<Tp, Tu>> $x): Lazy<Tp, Tu>
        {
            return $x->flatMap($x ==> $x);
        }

        final public function force(): Tu
        {
            return $this->x->match(
                    $f ==>
                    {
                        $this->x = Variant::right($f());
                        return $this->force();
                    },
                    $x ==> $x);
        }
    }

    type LazyT<+Tu> = Lazy<LazyPrx, Tu>;

}

