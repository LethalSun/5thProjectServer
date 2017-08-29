﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using CloudStructures;

namespace DBServer.DB
{
    public class Redis
    {
        static RedisGroup redisGroupBasic = null;

        public static ERROR_CODE Init(string address)
        {
            try
            {
                var basicRedisConnectString = address.Split(",").ToList();
                var redisSettings = new RedisSettings[basicRedisConnectString.Count];

                if (basicRedisConnectString.Count()>0)
                {
                    var redisSettingsBasic = new RedisSettings[basicRedisConnectString.Count()];

                    for(int i = 0;i<basicRedisConnectString.Count();++i)
                    {
                        redisSettings[i] = new RedisSettings(basicRedisConnectString[i], db: 0);
                    }

                    redisGroupBasic = new RedisGroup(groupName: "Basic", settings: redisSettings);
                }

                for(int i = 0;i<basicRedisConnectString.Count;++i)
                {
                    var key = i.ToString() + "_test";
                    var redis = new RedisString<int>(redisGroupBasic, key);
                    var result = redis.Set(11);
                    if(result.Result == false)
                    {
                        return ERROR_CODE.REDIS_START_SET_TEST;
                    }
                }

                return ERROR_CODE.NONE;
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return ERROR_CODE.REDIS_START_EXCEPTION;
            }
        }

        public static async Task<bool> SetStringAsync<T>(string key,T dataObject)
        {
            try
            {
                var redis = new RedisString<T>(redisGroupBasic, key);
                await redis.Set(dataObject);
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
        }

        public static void SetStringNoReturn<T>(string key,T dataObject)
        {
            try
            {
                var redis = new RedisString<T>(redisGroupBasic, key);
                redis.Set(dataObject);

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        public static async Task<bool> SetStringAsyncWhenNotExist<T>(string key,T dataObject)
        {
            try
            {
                var redis = new RedisString<T>(redisGroupBasic, key);
                var result = await redis.Set(dataObject, null, StackExchange.Redis.When.NotExists);
                return result;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
        }

        public static async Task<Tuple<bool,T>> GetStringAsync<T>(string key)
        {
            try
            {
                var redis = new RedisString<T>(redisGroupBasic, key);
                var value = await redis.Get();

                if (value.Value == null)
                {
                    return Tuple.Create(false, default(T));
                }

                return Tuple.Create(true, value.Value);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return Tuple.Create(false, default(T));
            }
        }

        public static Tuple<bool,T>GetString<T>(string key)
        {
            try
            {
                var redis = new RedisString<T>(redisGroupBasic, key);
                var value = redis.Get();

                if (value.Result.HasValue == false)
                {
                    return Tuple.Create(false, default(T));
                }

                return Tuple.Create(true, value.Result.Value);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Redis GetString() 예외: " + ex.Message);
                return Tuple.Create(false, default(T));
            }
        }

        public static async Task<Int64> IncrementAsync(string key,Int64 value)
        {
            try
            {
                var redis = new RedisString<Int64>(redisGroupBasic, key);
                var result = await redis.Increment(value);
                return result;
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return 0;
            }
        }

        public static Int64 Increment(string key,Int64 value)
        {
            try
            {
                var redis = new RedisString<Int64>(redisGroupBasic, key);
                var result = redis.Increment(value);
                return result.Result;
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return 0;
            }
        }

        public static async Task<bool> DeleteStringAsync<T>(string key)
        {
            try
            {
                var redis = new RedisString<T>(redisGroupBasic, key);
                var result = await redis.Delete();
                return result;
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
        }

        public static void DeleteStringNoReturn<T>(string key)
        {
            try
            {
                var redis = new RedisString<T>(redisGroupBasic, key);
                var result = redis.Delete();
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        public static async Task<long> AddListAsync<T>(string key, T value)
        {
            try
            {
                var redis = new RedisList<T>(redisGroupBasic, key);
                var result = await redis.LeftPush(value);
                return result;
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return 0;
            }
        }

        public static async Task<List<T>> GetListAsync<T>(string key,int startPos,int endPos)
        {
            try
            {
                var redis = new RedisList<T>(redisGroupBasic, key);
                var result = await redis.Range(startPos, endPos);
                return result.ToList();
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return null;
            }
        }

        public static async Task<List<T>>GetListAsync<T>(string key, int startPos)
        {
            try
            {
                var redis = new RedisList<T>(redisGroupBasic, key);
                var result = await redis.Range(startPos, -1);
                return result.ToList();
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return null;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="key"></param>
        /// <param name="value">이 값과 같은 것만 지운다</param>
        /// <param name="count">삭제할 개수. count가 0 보다 크면 왼쪽에서, count가 0 보다 작으면 오른쪽에서, 0과 같으면 모두 지운다</param>
        /// <returns></returns>
        /// 

        //키에 대해서 리스트로 저장된 것 중에서 위의 룰에서 제거.
        public static async Task<long> DeleteListAsync<T>(string key, T value, int count)
        {
            try
            {
                var redis = new RedisList<T>(redisGroupBasic, key);
                var result = await redis.Remove(value, count);
                return result;
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return 0;
            }
        }

        //키에 대해서 리스트로 저장된 것중 왼쪽이나 오른쪽에서 제거.
        public static async Task<T> DeleteListAsync<T>(string key,bool isLeft)
        {
            try
            {
                var redis = new RedisList<T>(redisGroupBasic, key);

                if(isLeft)
                {
                    var result = await redis.LeftPop();
                    return result.Value;
                }
                else
                {
                    var result = await redis.RightPop();
                    return result.Value;
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return default(T);
            }
        }
    }
}