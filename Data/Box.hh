<?hh // strict

namespace Data
{

    final class Box<+Tu>
    {
        final private function __construct(private Tu $x)
        {
        }

        final public static function make(Tu $x): Box<Tu>
        {
            return new self($x);
        }

        final public function match<Tv>((function (Tu): Tv) $f): Tv
        {
            return $f($this->x);
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Box<Tv>
        {
            return Box::pure($f($this->x));
        }

        final public static function pure(Tu $x): Box<Tu>
        {
            return Box::make($x);
        }

        final public static function flatten(Box<Box<Tu>> $x): Box<Tu>
        {
            return $x->flatMap($x ==> $x);
        }

        final public function flatMap<Tv>((function (Tu): Box<Tv>) $f): Box<Tv>
        {
            return $this->match($f);
        }
    }

}

