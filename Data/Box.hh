<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IMonad.hh');

    final class BoxPrx
    {
        final private function __construct()
        {
        }
    }

    final class Box<Tp as BoxPrx, +Tu> implements IMonad<Tp, Tu>
    {
        final private function __construct(private Tu $x)
        {
        }

        final public static function make(Tu $x): Box<Tp, Tu>
        {
            return new self($x);
        }

        final public function match<Tv>((function (Tu): Tv) $f): Tv
        {
            return $f($this->x);
        }

        final public function asFunctor<Tv>((function (IFunctor<Tp, Tu>): IFunctor<Tp, Tv>) $f): Box<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Box, 'unique proxy type');
            return $x;
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Box<Tp, Tv>
        {
            return Box::make($f($this->x));
        }

        final public function asMonad<Tv>((function (IMonad<Tp, Tu>): IMonad<Tp, Tv>) $f): Box<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Box, 'unique proxy type');
            return $x;
        }

        final public function flatMap<Tv>((function (Tu): IMonad<Tp, Tv>) $f): Box<Tp, Tv>
        {
            return $this->match($x ==>
                    {
                        $z = $f($x);
                        invariant($z instanceof Box, 'unique proxy type');
                        return $z;
                    });
        }

        final public function singleton<Tv>(Tv $x): Box<Tp, Tv>
        {
            return Box::make($x);
        }

        final public static function flatten(Box<Tp, Box<Tp, Tu>> $x): Box<Tp, Tu>
        {
            return $x->flatMap($x ==> $x);
        }
    }

    type BoxT<+Tu> = Box<BoxPrx, Tu>;

}

