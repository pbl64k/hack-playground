<?hh // strict

namespace Data
{

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

        final public function match<Tt>((function (): Tt) $f): Tt
        {
            return $f();
        }
    }

}

