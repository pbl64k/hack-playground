<?hh // strict

namespace Data
{

    require_once(__DIR__.'/Variant.hh');

    final class Lazy<+Tu>
    {
        final private function __construct(private VariantT<(function (): Tu), Tu> $x)
        {
        }

        final public static function delay((function (): Tu) $f): Lazy<Tu>
        {
            return new self(Variant::left($f));
        }

        final public static function promote(Tu $x): Lazy<Tu>
        {
            return new self(Variant::right($x));
        }

        final public function match<Tv>((function (Tu): Tv) $f): Tv
        {
            return $f($this->force());
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

        final public function fmap<Tv>((function (Tu): Tv) $f): Lazy<Tv>
        {
            return Lazy::delay(() ==> $f($this->force()));
        }

        final public static function pure(Tu $x): Lazy<Tu>
        {
            return Lazy::promote($x);
        }

        final public static function flatten(Lazy<Lazy<Tu>> $x): Lazy<Tu>
        {
            return $x->flatMap($x ==> $x);
        }

        final public function flatMap<Tv>((function (Tu): Lazy<Tv>) $f): Lazy<Tv>
        {
            return $this->match($f);
        }
    }

}

