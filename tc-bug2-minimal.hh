<?hh // strict

    final class Unit
    {
        final private function __construct()
        {
        }

        <<__Memoize>>
        final public static function make(): Unit
        {
            return new self();
        }
    }

    final class LazyPrx
    {
        final private function __construct()
        {
        }
    }

    final class Lazy<Tp as LazyPrx, +Tu>
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

        final public function fmap<Tv>((function (Tu): Tv) $f): Lazy<Tp, Tv>
        {
            return Lazy::delay(() ==> $f($this->force()));
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

    final class VariantPrx
    {
        final private function __construct()
        {
        }
    }

    abstract class Variant<Tp as VariantPrx, Tt, +Tu>
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

        final public function fmap<Tv>((function (Tu): Tv) $f): Variant<Tp, Tt, Tv>
        {
            return $this->bimap($x ==> $x, $f);
        }

        final public function bimap<Tv, Tw>((function (Tt): Tv) $f, (function (Tu): Tw) $g): Variant<Tp, Tv, Tw>
        {
            return $this->match($x ==> Variant::left($f($x)), $x ==> Variant::right($g($x)));
        }
    }

    final class Left<Tp as VariantPrx, Tt, +Tu> extends Variant<Tp, Tt, Tu>
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

    final class Right<Tp as VariantPrx, Tt, +Tu> extends Variant<Tp, Tt, Tu>
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

    type VariantT<Tt, +Tu> = Variant<VariantPrx, Tt, Tu>;

    final class Tuple2Prx
    {
        final private function __construct()
        {
        }
    }

    final class Tuple2<Tp as Tuple2Prx, +Tt, +Tu>
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

        final public function fmap<Tv>((function (Tu): Tv) $f): Tuple2<Tp, Tt, Tv>
        {
            return $this->bimap($x ==> $x, $f);
        }

        final public function bimap<Tv, Tw>((function (Tt): Tv) $f, (function (Tu): Tw) $g): Tuple2<Tp, Tv, Tw>
        {
            return $this->match(($x, $y) ==> Tuple2::make($f($x), $g($y)));
        }
    }

    type Tuple2T<+Tt, +Tu> = Tuple2<Tuple2Prx, Tt, Tu>;

    final class OptionalPrx
    {
        final private function __construct()
        {
        }
    }

    final class Optional<Tp as OptionalPrx, +Tu>
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

        final public function fmap<Tv>((function (Tu): Tv) $f): Optional<Tp, Tv>
        {
            return new self($this->x->fmap($f));
        }
    }

    type OptionalT<+Tu> = Optional<OptionalPrx, Tu>;

    final class StreamEmptyException extends \Exception
    {
    }

    final class StreamPrx
    {
        final private function __construct()
        {
        }
    }

    final class Stream<Tp as StreamPrx, +Tt>
    {
        final private function __construct(private OptionalT<Tuple2T<Tt, LazyT<Stream<Tp, Tt>>>> $xs)
        {
        }
        
        <<__Memoize>>
        final public static function nil(): Stream<Tp, Tt>
        {
            return new self(Optional::none());
        }

        final public static function strm(Tt $x, LazyT<Stream<Tp, Tt>> $xs): Stream<Tp, Tt>
        {
            return new self(Optional::some(Tuple2::make($x, $xs)));
        }

        final public function match<Tv>((function (): Tv) $ifEmpty, (function (Tt, LazyT<Stream<Tp, Tt>>): Tv) $ifStream): Tv
        {
            return $this->xs->match($ifEmpty, $x ==> $x->match($ifStream));
        }

        final public function lazyFold<Tv>((function (OptionalT<Tuple2T<Tt, LazyT<Tv>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lazy ==> $lazy->fmap($lst ==> $lst->lazyFold($f)))));
        }

        final public static function unfold<Tv>((function (Tv): OptionalT<Tuple2T<Tt, Tv>>) $f, Tv $x): Stream<Tp, Tt>
        {
            return new self($f($x)->fmap($pair ==> $pair->fmap($seed ==> Lazy::delay(() ==> Stream::unfold($f, $seed)))));
        }

        final public function take(int $n): Stream<Tp, Tt>
        {
            return Stream::unfold($pair ==> $pair->match(
                    ($n, $xs) ==>
                        $n <= 0
                            ? Optional::none()
                            : $xs->match(
                                () ==> Optional::none(),
                                ($x, $xs) ==> Optional::some(Tuple2::make($x, Tuple2::make($n - 1, $xs->force()))))),
                    Tuple2::make($n, $this));
        }

        // FIXME
        final public function takeIncorrect(int $n): Stream<Tp, Tt>
        {
            return Stream::unfold($pair ==> $pair->match(
                    ($n, $xs) ==>
                        $n <= 0
                            ? Optional::none()
                            : $xs->match(
                                () ==> Optional::none(),
                                // cf. take() above: $xs is of type Lazy<Stream<Tp, Tt>>, while a Stream<Tp, Tt> should be expected here
                                ($x, $xs) ==> Optional::some(Tuple2::make($x, Tuple2::make($n - 1, $xs))))),
                    Tuple2::make($n, $this));
        }
    }

    type StreamT<+Tt> = Stream<StreamPrx, Tt>;

    function test(): void
    {
        $nats = Stream::unfold($x ==> Optional::some(Tuple2::make($x, $x + 1)), 1);
        $nats->take(10)->lazyFold($x ==> $x->match(
                () ==> { print('End of stream.'."\n"); },
                $pair ==> $pair->match(($x, $xs) ==> { print('Element: '.$x."\n"); $xs->force(); })));
        $nats->takeIncorrect(10)->lazyFold($x ==> $x->match(
                () ==> { print('End of stream.'."\n"); },
                $pair ==> $pair->match(($x, $xs) ==> { print('Element: '.$x."\n"); $xs->force(); })));
    }
    
