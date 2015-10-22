<?hh // strict

namespace Data
{

    interface IFunctor<Tp, +Tu>
    {
        public function fmap<Tv>((function (Tu): Tv) $f): IFunctor<Tp, Tv>;
    }

}

