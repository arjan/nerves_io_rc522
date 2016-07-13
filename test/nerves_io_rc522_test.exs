defmodule NervesIoRc522Test do
  use ExUnit.Case

  test "RC522 port process talks to us in test mode" do

    parent = self()

    {:ok, _pid} = Nerves.IO.RC522.start_link(fn(tag) ->
      send(parent, {:tag, tag})
    end)

    receive do
      {:tag, _tag} ->
        :ok
    after 1000 ->
        assert false
    end

  end
end
