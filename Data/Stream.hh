<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IMonad.hh');
    require_once(__DIR__.'/Lazy.hh');
    require_once(__DIR__.'/Optional.hh');
    require_once(__DIR__.'/Variant.hh');
    require_once(__DIR__.'/Tuple2.hh');

    final class StreamEmptyException extends \Exception
    {
    }

    final class StreamPrx
    {
        final private function __construct()
        {
        }
    }

    final class Stream<Tp as StreamPrx, +Tt> implements IMonad<Tp, Tt>
    {
        final private function __construct(private OptionalT<Tuple2T<Tt, LazyT<Stream<Tp, Tt>>>> $xs)
        {
        }
        
        <<__Memoize>>
        final public static function nil(): Stream<Tp, Tt>
        {
            return new self(Optional::none());
        }

        final public static function stream(Tt $x, LazyT<Stream<Tp, Tt>> $xs): Stream<Tp, Tt>
        {
            return new self(Optional::some(Tuple2::make($x, $xs)));
        }

        final public function match<Tv>((function (): Tv) $ifEmpty, (function (Tt, LazyT<Stream<Tp, Tt>>): Tv) $ifStream): Tv
        {
            return $this->xs->match($ifEmpty, $x ==> $x->match($ifStream));
        }

        final public function asFunctor<Tv>((function (IFunctor<Tp, Tt>): IFunctor<Tp, Tv>) $f): Stream<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Stream, 'unique proxy type');
            return $x;
        }

        final public function fmap<Tv>((function (Tt): Tv) $f): Stream<Tp, Tv>
        {
            return $this->match(() ==> Stream::nil(), ($x, $xs) ==> Stream::stream($f($x), $xs->fmap($xs ==> $xs->fmap($f))));
        }

        final public function asMonad<Tv>((function (IMonad<Tp, Tt>): IMonad<Tp, Tv>) $f): Stream<Tp, Tv>
        {
            $x = $f($this);
            invariant($x instanceof Stream, 'unique proxy type');
            return $x;
        }

        final public function flatMap<Tv>((function (Tt): IMonad<Tp, Tv>) $f): Stream<Tp, Tv>
        {
            return $this->match(() ==> Stream::nil(), ($x, $xs) ==>
                    {
                        $z = $f($x);
                        invariant($z instanceof Stream, 'unique proxy type');
                        return Stream::lazyAppend($z, $xs->fmap($xs ==> $xs->flatMap($f)));
                    });
        }

        final public function singleton<Tv>(Tv $x): Stream<Tp, Tv>
        {
            return Stream::stream($x, Lazy::promote(Stream::nil()));
        }

        final public static function flatten(Stream<Tp, Stream<Tp, Tt>> $xs): Stream<Tp, Tt>
        {
            return $xs->lazyFold($opt ==> $opt->match(() ==> Stream::nil(), $pair ==> $pair->match(($x, $xs) ==> Stream::lazyAppend($x, $xs))));
        }

        final public function lazyFold<Tv>((function (OptionalT<Tuple2T<Tt, LazyT<Tv>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lazy ==> $lazy->fmap($lst ==> $lst->lazyFold($f)))));
        }

        final public static function unfold<Tv>((function (Tv): OptionalT<Tuple2T<Tt, LazyT<Tv>>>) $f, Tv $x): Stream<Tp, Tt>
        {
            return new self($f($x)->fmap($pair ==> $pair->fmap($seed ==> $seed->fmap($z ==> Stream::unfold($f, $z)))));
        }

        final public function lazyPara<Tv>((function (OptionalT<Tuple2T<Tt, LazyT<Tuple2T<Tv, Stream<Tp, Tt>>>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lazy ==> $lazy->fmap($str ==> Tuple2::make($str->lazyPara($f), $str)))));
        }

        final public static function apo<Tv>((function (Tv): OptionalT<Tuple2T<Tt, LazyT<VariantT<Tv, Stream<Tp, Tt>>>>>) $f, Tv $x): Stream<Tp, Tt>
        {
            return new self($f($x)->fmap($pair ==> $pair->fmap($lazy ==> $lazy->fmap($next ==> $next->match($seed ==> Stream::apo($f, $seed), $x ==> $x)))));
        }

        final public function zip2<Tv>(Stream<Tp, Tv> $ys): Stream<Tp, Tuple2T<Tt, Tv>>
        {
            return $this->zipWith2($ys, ($x, $y) ==> Tuple2::make($x, $y));
        }

        final public function zipWith2<Tv, Tw>(Stream<Tp, Tv> $ys, (function (Tt, Tv): Tw) $f): Stream<Tp, Tw>
        {
            return Stream::unfold(
                    $pair ==> $pair->match(
                        ($xs, $ys) ==> $xs->match(
                            () ==> Optional::none(),
                            ($x, $xs) ==> $ys->match(
                                () ==> Optional::none(),
                                ($y, $ys) ==> Optional::some(Tuple2::make($f($x, $y), Lazy::delay(() ==> Tuple2::make($xs->force(), $ys->force()))))))),
                    Tuple2::make($this, $ys));
        }

        <<__Memoize>>
        final public function isEmpty(): bool
        {
            return $this->match(() ==> true, ($x, $xs) ==> false);
        }

        <<__Memoize>>
        final public function first(): OptionalT<Tt>
        {
            return $this->xs->fmap($x ==> $x->first());
        }

        <<__Memoize>>
        final public function rest(): OptionalT<Stream<Tp, Tt>>
        {
            return $this->xs->fmap($x ==> $x->second()->force());
        }

        final public static function lazyAppend(Stream<Tp, Tt> $xs, LazyT<Stream<Tp, Tt>> $ys): Stream<Tp, Tt>
        {
            return $xs->match(() ==> $ys->force(), ($x, $xs) ==> Stream::stream($x, Lazy::delay(() ==> Stream::lazyAppend($xs->force(), $ys))));
        }

        final public function take(int $n): Stream<Tp, Tt>
        {
            return Stream::unfold($pair ==> $pair->match(
                    ($n, $xs) ==>
                        $n <= 0
                            ? Optional::none()
                            : $xs->match(
                                () ==> Optional::none(),
                                ($x, $xs) ==> Optional::some(Tuple2::make($x, Lazy::delay(() ==> Tuple2::make($n - 1, $xs->force())))))),
                    Tuple2::make($n, $this));
        }
    }

    type StreamT<+Tt> = Stream<StreamPrx, Tt>;

}

