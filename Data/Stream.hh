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

        final public static function lazyAppend(Stream<Tp, Tt> $xs, LazyT<Stream<Tp, Tt>> $ys): Stream<Tp, Tt>
        {
            return $xs->match(() ==> $ys->force(), ($x, $xs) ==> Stream::stream($x, Lazy::delay(() ==> Stream::lazyAppend($xs->force(), $ys))));
        }

        final public static function flatten(Stream<Tp, Stream<Tp, Tt>> $xs): Stream<Tp, Tt>
        {
            return $xs->lazyFold($opt ==> $opt->match(() ==> Stream::nil(), $pair ==> $pair->match(($x, $xs) ==> Stream::lazyAppend($x, $xs))));
        }

        //final public function fold<Tv>((function (OptionalT<Tuple2T<Tt, Tv>>): Tv) $f): Tv
        //{
        //    return $f($this->xs->fmap($pair ==> $pair->fmap($lst ==> $lst->fold($f))));
        //}

        final public function lazyFold<Tv>((function (OptionalT<Tuple2T<Tt, LazyT<Tv>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lazy ==> $lazy->fmap($lst ==> $lst->lazyFold($f)))));
        }

        final public static function unfold<Tv>((function (Tv): OptionalT<Tuple2T<Tt, Tv>>) $f, Tv $x): Stream<Tp, Tt>
        {
            return new self($f($x)->fmap($pair ==> $pair->fmap($seed ==> Lazy::delay(() ==> Stream::unfold($f, $seed)))));
        }

        //final public function para<Tv>((function (OptionalT<Tuple2T<Tt, Tuple2T<Tv, ConsList<Tp, Tt>>>>): Tv) $f): Tv
        //{
        //    return $f($this->xs->fmap($pair ==> $pair->fmap($lst ==> Tuple2::make($lst->para($f), $lst))));
        //}

        //final public function lazyPara<Tv>((function (OptionalT<Tuple2T<Tt, Tuple2T<LazyT<Tv>, ConsList<Tp, Tt>>>>): Tv) $f): Tv
        //{
        //    return $f($this->xs->fmap($pair ==> $pair->fmap($lst ==> Tuple2::make(Lazy::delay(() ==> $lst->lazyPara($f)), $lst))));
        //}

        //final public static function apo<Tv>((function (Tv): OptionalT<Tuple2T<Tt, VariantT<Tv, ConsList<Tp, Tt>>>>) $f, Tv $x): ConsList<Tp, Tt>
        //{
        //    return new self($f($x)->fmap($pair ==> $pair->fmap($next ==> $next->match($seed ==> ConsList::apo($f, $seed), $x ==> $x))));
        //}

        //final public function reduce<Tv>(Tv $nil, (function(Tt, Tv): Tv) $lst): Tv
        //{
        //    return $this->fold($x ==> $x->match(() ==> $nil, $x ==> $x->match($lst)));
        //}

        //final public function reduceLeft<Tv>(Tv $init, (function(Tt, Tv): Tv) $comp): Tv
        //{
        //    $lst = $this;
        //    $acc = $init;

        //    while (! $lst->isEmpty())
        //    {
        //        $acc = $comp($lst->unsafeFirst(), $acc);
        //        $lst = $lst->unsafeRest();
        //    }

        //    return $acc;
        //}

        //final public function zipWith2<Tv, Tw>(ConsList<Tp, Tv> $ys, (function (Tt, Tv): Tw) $f): ConsList<Tp, Tw>
        //{
        //    return ConsList::unfold(
        //            $pair ==> $pair->match(
        //                ($xs, $ys) ==> $xs->match(
        //                    () ==> Optional::none(),
        //                    ($x, $xs) ==> $ys->match(
        //                        () ==> Optional::none(),
        //                        ($y, $ys) ==> Optional::some(Tuple2::make($f($x, $y), Tuple2::make($xs, $ys)))))),
        //            Tuple2::make($this, $ys));
        //}

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

        //final private function unsafeFirst(): Tt
        //{
        //    return $this->match(() ==> { throw new ConsListEmptyException(); }, ($x, $xs) ==> $x);
        //}

        //final private function unsafeRest(): ConsList<Tp, Tt>
        //{
        //    return $this->match(() ==> { throw new ConsListEmptyException(); }, ($x, $xs) ==> $xs);
        //}

        //final public static function append(ConsList<Tp, Tt> $xs, ConsList<Tp, Tt> $ys): ConsList<Tp, Tt>
        //{
        //    return $ys->reduceLeft($xs->reverse(), ($x, $xs) ==> ConsList::lst($x, $xs))->reverse();
        //}

        //<<__Memoize>>
        //final public function length(): int
        //{
        //    return $this->reduceLeft(0, ($x, $xs) ==> $xs + 1);
        //}

        //<<__Memoize>>
        //final public function reverse(): ConsList<Tp, Tt>
        //{
        //    return $this->reduceLeft(ConsList::nil(), ($x, $xs) ==> ConsList::lst($x, $xs));
        //}

        //final public function forall((function (Tt): bool) $p): bool
        //{
        //    return $this->lazyFold($opt ==> $opt->match(() ==> true, $pair ==> $pair->match(($x, $y) ==> $p($x) && $y->force())));
        //}

        //final public function exists((function (Tt): bool) $p): bool
        //{
        //    return !$this->forall($x ==> !$p($x));
        //}

        final public function take(int $n): Stream<Tp, Tt>
        {
            return Stream::unfold($pair ==> $pair->match(
                    ($n, $xs) ==>
                        $n <= 0
                            ? Optional::none()
                            : $xs->match(
                                () ==> Optional::none(),
                                ($x, $xs) ==> Optional::some(Tuple2::make($x, Tuple2::make($n - 1, $xs->force()))))),
                                //($x, $xs) ==> Optional::some(Tuple2::make($x, Tuple2::make($n - 1, $xs))))),
                    Tuple2::make($n, $this));
        }
    }

    type StreamT<+Tt> = Stream<StreamPrx, Tt>;

}

