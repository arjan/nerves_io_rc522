defmodule Nerves.IO.RC522 do

  use Application

  def start(_type, _args) do
    import Supervisor.Spec

    children = [
      worker(Nerves.IO.RC522.Worker, [])
    ]
    Supervisor.start_link(children, strategy: :one_for_one)
  end


  defmodule Worker do
    @moduledoc """
    Worker module that spawns the RC522 port process and handles all communication.
    """

    use GenServer

    require Logger

    def start_link do
      GenServer.start_link(__MODULE__, [], name: __MODULE__)
    end


    defmodule State do
      @moduledoc false
      defstruct port: nil
    end

    def init([]) do
      Logger.info "RC522 worker starting"

      executable = :code.priv_dir(:nerves_io_rc522) ++ '/rc522'
      port = Port.open({:spawn_executable, executable},
                       [{:args, [Integer.to_string(1000000)]},
                        {:packet, 2},
                        :use_stdio,
                        :binary,
                        :exit_status])
      state = %State{port: port}
      {:ok, state}
    end

    def handle_info({port, {:data, data}}, state = %State{port: port}) do
      msg = :erlang.binary_to_term(data)
      Logger.info "Info!! #{inspect msg}"
      {:noreply, state}
    end

  end
end
