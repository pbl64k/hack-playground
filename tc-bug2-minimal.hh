<?hh // strict

    final class Lazy<+Tu>
    {
        final public function __construct(private (function (): Tu) $x)
        {
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Lazy<Tv>
        {
            return new Lazy(() ==> $f($this->force()));
        }

        final public function force(): Tu
        {
            $f = $this->x;
            $x = $f();
            $this->x = () ==> $x;
            return $x;
        }
    }

    final class Tuple2<+Tt, +Tu>
    {
        final public function __construct(private Tt $first, private Tu $second)
        {
        }

        final public function match<Tv>((function (Tt, Tu): Tv) $f): Tv
        {
            return $f($this->first, $this->second);
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Tuple2<Tt, Tv>
        {
            return $this->match(($x, $y) ==> new Tuple2($x, $f($y)));
        }
    }

    final class Optional<+Tu>
    {
        final public function __construct(private ?Tu $x)
        {
        }

        final public function match<Tv>((function (): Tv) $ifNone, (function (Tu): Tv) $ifSome): Tv
        {
            return is_null($this->x) ? $ifNone() : $ifSome($this->x);
        }

        final public function fmap<Tv>((function (Tu): Tv) $f): Optional<Tv>
        {
            return $this->match(() ==> new Optional(null), $x ==> new Optional($f($x)));
        }
    }

    final class Stream<+Tt>
    {
        final private function __construct(private Optional<Tuple2<Tt, Lazy<Stream<Tt>>>> $xs)
        {
        }
        
        final public function match<Tv>((function (): Tv) $ifEmpty, (function (Tt, Lazy<Stream<Tt>>): Tv) $ifStream): Tv
        {
            return $this->xs->match($ifEmpty, $x ==> $x->match($ifStream));
        }

        final public function lazyFold<Tv>((function (Optional<Tuple2<Tt, Lazy<Tv>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lazy ==> $lazy->fmap($lst ==> $lst->lazyFold($f)))));
        }

        final public static function unfold<Tv>((function (Tv): Optional<Tuple2<Tt, Tv>>) $f, Tv $x): Stream<Tt>
        {
            return new self($f($x)->fmap($pair ==> $pair->fmap($seed ==> new Lazy(() ==> Stream::unfold($f, $seed)))));
        }

        final public function take(int $n): Stream<Tt>
        {
            return Stream::unfold($pair ==> $pair->match(
                    ($n, $xs) ==>
                        $n <= 0
                            ? new Optional(null)
                            : $xs->match(
                                () ==> new Optional(null),
                                ($x, $xs) ==> new Optional(new Tuple2($x, new Tuple2($n - 1, $xs->force()))))),
                    new Tuple2($n, $this));
        }

        // FIXME
        final public function takeIncorrect(int $n): Stream<Tt>
        {
            return Stream::unfold($pair ==> $pair->match(
                    ($n, $xs) ==>
                        $n <= 0
                            ? new Optional(null)
                            : $xs->match(
                                () ==> new Optional(null),
                                // cf. take() above: $xs is of type Lazy<Stream<Tt>>, while a Stream<Tt> should be expected here
                                ($x, $xs) ==> new Optional(new Tuple2($x, new Tuple2($n - 1, $xs))))),
                    new Tuple2($n, $this));
        }
    }

    function test(): void
    {
        $nats = Stream::unfold($x ==> new Optional(new Tuple2($x, $x + 1)), 1);
        $nats->take(10)->lazyFold($x ==> $x->match(
                () ==> { print('End of stream.'."\n"); },
                $pair ==> $pair->match(($x, $xs) ==> { print('Element: '.$x."\n"); $xs->force(); })));
        $nats->takeIncorrect(10)->lazyFold($x ==> $x->match(
                () ==> { print('End of stream.'."\n"); },
                $pair ==> $pair->match(($x, $xs) ==> { print('Element: '.$x."\n"); $xs->force(); })));
    }
    
