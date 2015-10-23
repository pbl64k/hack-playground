<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IMonad.hh');
    require_once(__DIR__.'/Variant.hh');
    require_once(__DIR__.'/Unit.hh');

    final class OptionalPrx
    {
        final private function __construct()
        {
        }
    }

    final class Optional<Tp as OptionalPrx, +Tu> implements IMonad<Tp, Tu>
    {
        final private function __construct(private VariantT<Unit, Tu> $x)
        {
        }

        <<__Memoize>>
        final public static function none(): Optional<Tp, Tu>
        {
            return new self(Variant::left(Unit::make()));
        }

        final public static function some(Tu $x): Optional<Tp, Tu>
        {
            return new self(Variant::right($x));
        }

        final public function match<Tv>((function (): Tv) $ifNone, (function (Tu): Tv) $ifSome): Tv
        {
            return $this->x->match($x ==> $ifNone(), $ifSome);
        }

        final public function asFunctor<Tv>((function (IFunctor<Tp, Tu>): IFunctor<Tp, Tv>) $f): Optional<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Optional, 'unique proxy type');
            return $x;
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Optional<Tp, Tv>
        {
            return new self($this->x->fmap($f));
        }

        final public function asMonad<Tv>((function (IMonad<Tp, Tu>): IMonad<Tp, Tv>) $f): Optional<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Optional, 'unique proxy type');
            return $x;
        }

        final public function flatMap<Tv>((function (Tu): IMonad<Tp, Tv>) $f): Optional<Tp, Tv>
        {
            return $this->match(() ==> Optional::none(), $x ==>
                    {
                        $z = $f($x);
                        invariant($z instanceof Optional, 'unique proxy type');
                        return $z;
                    });
        }

        final public function singleton<Tv>(Tv $x): Optional<Tp, Tv>
        {
            return Optional::some($x);
        }

        final public static function flatten(Optional<Tp, Optional<Tp, Tu>> $x): Optional<Tp, Tu>
        {
            return $x->flatMap($x ==> $x);
        }

        final public function toNullable(): ?Tu
        {
            return $this->x->match($x ==> null, $x ==> $x);
        }

        final public static function fromNullable(?Tu $x): Optional<Tp, Tu>
        {
            return is_null($x) ? Optional::none() : Optional::some($x);
        }

        final public static function withDefault(Optional<Tp, Tu> $opt, Tu $def): Tu
        {
            return $opt->match(() ==> $def, $x ==> $x);
        }
    }

    type OptionalT<+Tu> = Optional<OptionalPrx, Tu>;

}

