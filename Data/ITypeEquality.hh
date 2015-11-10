<?hh // strict

namespace Data
{

    interface ITypeEquality<Tt, Tu>
    {
        public function toRight(Tt $x): Tu;
        public function toLeft(Tu $x): Tt;
    }

}

