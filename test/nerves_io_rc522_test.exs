defmodule NervesIoRc522Test do
  use ExUnit.Case

  test "RC522 process has been started" do
    assert is_pid(Process.whereis(Nerves.IO.RC522.Worker))
    :timer.sleep(2000)
  end
end
