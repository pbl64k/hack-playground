<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IMonad.hh');
    require_once(__DIR__.'/Lazy.hh');
    require_once(__DIR__.'/Optional.hh');
    require_once(__DIR__.'/Variant.hh');
    require_once(__DIR__.'/Tuple2.hh');

    final class ConsListEmptyException extends \Exception
    {
    }

    final class ConsListImpossibleException extends \Exception
    {
    }

    final class ConsListPrx
    {
    }

    final class ConsList<Tp as ConsListPrx, +Tt> implements IMonad<Tp, Tt>
    {
        final private function __construct(private Optional<Tuple2<Tt, ConsList<Tp, Tt>>> $xs)
        {
        }
        
        <<__Memoize>>
        final public static function nil(): ConsList<Tp, Tt>
        {
            return new self(Optional::none());
        }

        final public static function lst(Tt $x, ConsList<Tp, Tt> $xs): ConsList<Tp, Tt>
        {
            return new self(Optional::some(Tuple2::make($x, $xs)));
        }

        final public function match<Tv>((function (): Tv) $ifEmpty, (function (Tt, ConsList<Tp, Tt>): Tv) $ifLst): Tv
        {
            return $this->xs->match($ifEmpty, $x ==> $x->match($ifLst));
        }

        final public static function fromFunctor(IFunctor<Tp, Tt> $x): ConsList<Tp, Tt>
        {
            if ($x instanceof ConsList)
            {
                return $x;
            }
            else
            {
                throw new ConsListImpossibleException();
            }
        }

        final public function fmap<Tv>((function (Tt): Tv) $f): ConsList<Tp, Tv>
        {
            return $this->reduceLeft(ConsList::nil(), ($x, $xs) ==> ConsList::lst($f($x), $xs))->reverse();
        }

        final public static function fromMonad(IMonad<Tp, Tt> $x): ConsList<Tp, Tt>
        {
            if ($x instanceof ConsList)
            {
                return $x;
            }
            else
            {
                throw new ConsListImpossibleException();
            }
        }

        final public static function pure(Tt $x): ConsList<Tp, Tt>
        {
            return ConsList::lst($x, ConsList::nil());
        }

        final public function flatMap<Tv>((function (Tt): IMonad<Tp, Tv>) $f): ConsList<Tp, Tv>
        {
            return ConsList::flatten($this->fmap($x ==> ConsList::fromMonad($f($x))));
        }

        final public static function flatten(ConsList<Tp, ConsList<Tp, Tt>> $xs): ConsList<Tp, Tt>
        {
            return $xs->reduce(ConsList::nil(), ($x, $xs) ==> ConsList::append($x, $xs));
        }

        final public function fold<Tv>((function (Optional<Tuple2<Tt, Tv>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lst ==> $lst->fold($f))));
        }

        final public function lazyFold<Tv>((function (Optional<Tuple2<Tt, Lazy<Tv>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lst ==> Lazy::delay(() ==> $lst->lazyFold($f)))));
        }

        final public static function unfold<Tv>((function (Tv): Optional<Tuple2<Tt, Tv>>) $f, Tv $x): ConsList<Tp, Tt>
        {
            return new self($f($x)->fmap($pair ==> $pair->fmap($seed ==> ConsList::unfold($f, $seed))));
        }

        final public function para<Tv>((function (Optional<Tuple2<Tt, Tuple2<Tv, ConsList<Tp, Tt>>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lst ==> Tuple2::make($lst->para($f), $lst))));
        }

        final public function lazyPara<Tv>((function (Optional<Tuple2<Tt, Tuple2<Lazy<Tv>, ConsList<Tp, Tt>>>>): Tv) $f): Tv
        {
            return $f($this->xs->fmap($pair ==> $pair->fmap($lst ==> Tuple2::make(Lazy::delay(() ==> $lst->lazyPara($f)), $lst))));
        }

        final public static function apo<Tv>((function (Tv): Optional<Tuple2<Tt, Variant<Tv, ConsList<Tp, Tt>>>>) $f, Tv $x): ConsList<Tp, Tt>
        {
            return new self($f($x)->fmap($pair ==> $pair->fmap($next ==> $next->match($seed ==> ConsList::apo($f, $seed), $x ==> $x))));
        }

        final public function reduce<Tv>(Tv $nil, (function(Tt, Tv): Tv) $lst): Tv
        {
            return $this->fold($x ==> $x->match(() ==> $nil, $x ==> $x->match($lst)));
        }

        final public function reduceLeft<Tv>(Tv $init, (function(Tt, Tv): Tv) $comp): Tv
        {
            $lst = $this;
            $acc = $init;

            while (! $lst->isEmpty())
            {
                $acc = $comp($lst->unsafeHead(), $acc);
                $lst = $lst->unsafeTail();
            }

            return $acc;
        }

        final public function zipWith2<Tv, Tw>(ConsList<Tp, Tv> $ys, (function (Tt, Tv): Tw) $f): ConsList<Tp, Tw>
        {
            return ConsList::unfold(
                    $pair ==> $pair->match(
                        ($xs, $ys) ==> $xs->match(
                            () ==> Optional::none(),
                            ($x, $xs) ==> $ys->match(
                                () ==> Optional::none(),
                                ($y, $ys) ==> Optional::some(Tuple2::make($f($x, $y), Tuple2::make($xs, $ys)))))),
                    Tuple2::make($this, $ys));
        }

        <<__Memoize>>
        final public function isEmpty(): bool
        {
            return $this->match(() ==> true, ($x, $xs) ==> false);
        }

        <<__Memoize>>
        final public function head(): Optional<Tt>
        {
            return $this->xs->fmap($x ==> $x->first());
        }

        <<__Memoize>>
        final public function tail(): Optional<ConsList<Tp, Tt>>
        {
            return $this->xs->fmap($x ==> $x->second());
        }

        final private function unsafeHead(): Tt
        {
            return $this->match(() ==> { throw new ConsListEmptyException(); }, ($x, $xs) ==> $x);
        }

        final private function unsafeTail(): ConsList<Tp, Tt>
        {
            return $this->match(() ==> { throw new ConsListEmptyException(); }, ($x, $xs) ==> $xs);
        }

        final public static function append(ConsList<Tp, Tt> $xs, ConsList<Tp, Tt> $ys): ConsList<Tp, Tt>
        {
            return $xs->reduce($ys, ($x, $xs) ==> ConsList::lst($x, ConsList::append($xs, $ys)));
        }

        <<__Memoize>>
        final public function length(): int
        {
            return $this->reduceLeft(0, ($x, $xs) ==> $xs + 1);
        }

        <<__Memoize>>
        final public function reverse(): ConsList<Tp, Tt>
        {
            return $this->reduceLeft(ConsList::nil(), ($x, $xs) ==> ConsList::lst($x, $xs));
        }
    }

}
