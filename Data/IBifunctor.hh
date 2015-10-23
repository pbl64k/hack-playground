<?hh // strict

namespace Data
{

    interface IBifunctor<Tp, +Tt, +Tu>
    {
        public function bimap<Tv, Tw>((function (Tt): Tv) $f, (function (Tu): Tw) $g): IBifunctor<Tp, Tv, Tw>;
    }

}

