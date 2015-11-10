<?hh // strict

namespace Data
{

    require_once(__DIR__.'/ITypeEquality.hh');
    require_once(__DIR__.'/IMonad.hh');

    final class ReflexivityPrx
    {
        final private function __construct()
        {
        }
    }

    final class Reflexivity<Tp as ReflexivityPrx, Tu> implements ITypeEquality<Tu, Tu>, IMonad<Tp, Tu>
    {
        final private function __construct()
        {
        }

        final public static function make(): Reflexivity<Tp, Tu>
        {
            return new self();
        }

        final public function match<Tv>((function (): Tv) $f): Tv
        {
            return $f();
        }

        final public function toRight(Tu $x): Tu
        {
            return $x;
        }

        final public function toLeft(Tu $x): Tu
        {
            return $x;
        }

        final public function asFunctor<Tv>((function (IFunctor<Tp, Tu>): IFunctor<Tp, Tv>) $f): Reflexivity<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Reflexivity, 'unique proxy type');
            return $x;
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Reflexivity<Tp, Tv>
        {
            return Reflexivity::make();
        }

        final public function asMonad<Tv>((function (IMonad<Tp, Tu>): IMonad<Tp, Tv>) $f): Reflexivity<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Reflexivity, 'unique proxy type');
            return $x;
        }

        final public function flatMap<Tv>((function (Tu): IMonad<Tp, Tv>) $f): Reflexivity<Tp, Tv>
        {
            return Reflexivity::make();
        }

        final public function singleton<Tv>(Tv $x): Reflexivity<Tp, Tv>
        {
            return Reflexivity::make();
        }
    }

    type ReflexivityT<Tu> = Reflexivity<ReflexivityPrx, Tu>;

}

