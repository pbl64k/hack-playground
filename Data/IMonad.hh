<?hh // strict

namespace Data
{

    require_once(__DIR__.'/IFunctor.hh');

    interface IMonad<Tp, +Tu> extends IFunctor<Tp, Tu>
    {
        public function flatMap<Tv>((function (Tu): IMonad<Tp, Tv>) $f): IMonad<Tp, Tv>;
        public function singleton<Tv>(Tv $x): IMonad<Tp, Tv>;
    }

}

