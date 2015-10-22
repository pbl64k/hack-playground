<?hh // strict

namespace Data
{

    require_once(__DIR__.'/Variant.hh');
    require_once(__DIR__.'/Unit.hh');

    final class Optional<+Tu>
    {
        final private function __construct(private Variant<Unit, Tu> $x)
        {
        }

        <<__Memoize>>
        final public static function none(): Optional<Tu>
        {
            return new self(Variant::left(Unit::make()));
        }

        final public static function some(Tu $x): Optional<Tu>
        {
            return new self(Variant::right($x));
        }

        final public function match<Tv>((function (): Tv) $ifNone, (function (Tu): Tv) $ifSome): Tv
        {
            return $this->x->match($x ==> $ifNone(), $ifSome);
        }

        final public function toNullable(): ?Tu
        {
            return $this->x->match($x ==> null, $x ==> $x);
        }

        final public static function fromNullable(?Tu $x): Optional<Tu>
        {
            return is_null($x) ? Optional::none() : Optional::some($x);
        }

        final public static function withDefault(Optional<Tu> $opt, Tu $def): Tu
        {
            return $opt->match(() ==> $def, $x ==> $x);
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Optional<Tv>
        {
            return new self($this->x->fmap($f));
        }

        final public static function pure(Tu $x): Optional<Tu>
        {
            return Optional::some($x);
        }

        final public static function flatten(Optional<Optional<Tu>> $x): Optional<Tu>
        {
            return $x->flatMap($x ==> $x);
        }

        final public function flatMap<Tv>((function (Tu): Optional<Tv>) $f): Optional<Tv>
        {
            return $this->match(() ==> Optional::none(), $f);
        }
    }

}

