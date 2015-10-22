<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IFunctor.hh');

    interface IMonad<Tp, +Tu> extends IFunctor<Tp, Tu>
    {
        public static function pure(Tu $x): IMonad<Tp, Tu>;
        public function flatMap<Tv>((function (Tu): IMonad<Tp, Tv>) $f): IMonad<Tp, Tv>;
    }

}

