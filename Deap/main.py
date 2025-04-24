import json
import random
import common_pb2
import common_pb2_grpc
import grpc
from concurrent import futures

from deap import base, creator, tools
import numpy as np
import matplotlib.pyplot as plt
import elitism

def get_score(w):
    with grpc.insecure_channel("localhost:50052") as channel:
        stub = common_pb2_grpc.CppStub(channel)
        response = stub.CountScore(common_pb2.Individual(w=w))

    # print("first_w", w[0], " score", response.value);

    return response.value,


def genetic(params):
    print(params)
    LOW = -1
    UP = 1
    ETA = 0.5

    LENGTH = params.wSize
    MAX_GEN = params.maxGen
    POP_SIZE = params.popSize
    HOF_SIZE = params.hofSize
    RAND_SEED = params.randSeed
    P_CROSS = params.pCross
    P_MUT = params.pMut

    hof = tools.HallOfFame(HOF_SIZE)
    random.seed(RAND_SEED)

    creator.create("FitnessMax", base.Fitness, weights=(1.0,))
    creator.create("Individual", list, fitness=creator.FitnessMax)

    toolbox = base.Toolbox()

    toolbox.register("randomWeight", random.uniform, LOW, UP)
    toolbox.register(
        "individualCreator",
        tools.initRepeat,
        creator.Individual,
        toolbox.randomWeight,
        LENGTH,
    )
    toolbox.register(
        "populationCreator", tools.initRepeat, list, toolbox.individualCreator
    )

    population = toolbox.populationCreator(n=POP_SIZE)

    toolbox.register("evaluate", get_score)
    toolbox.register("select", tools.selTournament, tournsize=2)
    toolbox.register("mate", tools.cxSimulatedBinaryBounded, low=LOW, up=UP, eta=ETA)
    toolbox.register(
        "mutate",
        tools.mutPolynomialBounded,
        low=LOW,
        up=UP,
        eta=ETA,
        indpb=1.0 / LENGTH,
    )

    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats.register("max", np.max)
    stats.register("avg", np.mean)

    population, logbook = elitism.ea_simple_elitism(
        population,
        toolbox,
        cxpb=P_CROSS,
        mutpb=P_MUT,
        ngen=MAX_GEN,
        halloffame=hof,
        stats=stats,
        verbose=True,
    )

    maxFitnessValues, meanFitnessValues = logbook.select("max", "avg")

    print('send best')

    with grpc.insecure_channel("localhost:50052") as channel:
        stub = common_pb2_grpc.CppStub(channel)
        response = stub.SaveBest(common_pb2.Individual(w=hof[0]))

    with open('hof[0].json', 'w') as file:
        file.write(json.dumps((hof[0])))

    with open('hof[1].json', 'w') as file:
        file.write(json.dumps((hof[1])))

    with open('population.json', 'w') as file:
        file.write(json.dumps(population))

    with open('logbook.json', 'w') as file:
        file.write(json.dumps(logbook))

    plt.plot(maxFitnessValues, color="red")
    plt.plot(meanFitnessValues, color="green")
    plt.xlabel("gen")
    plt.ylabel("max/avg")
    plt.savefig("plt.png")

    print('ready')


class PythonService(common_pb2_grpc.PythonServicer):
    def Genetic(self, request, context):
        genetic(request)
        # with grpc.insecure_channel("localhost:50052") as channel:
        #     stub = common_pb2_grpc.CppStub(channel)
        #     response = stub.CountScore(common_pb2.Individual(w=[1,2,3]))
        return common_pb2.Status(ok=True)


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=2))
    common_pb2_grpc.add_PythonServicer_to_server(PythonService(), server)
    server.add_insecure_port("localhost:50051")
    server.start()
    print("started")
    server.wait_for_termination()


if __name__ == "__main__":
    serve()
