#pragma once
#include "../pubh.h"
#include "../Reflection/StaticReflection.h"

namespace ZXEngine
{
	namespace Serialization
	{
		namespace Internal
		{
			template <typename T>
			struct SerializeHelper
			{
				static json SerializeToJson(const T& object)
				{
					json data;
					auto typeInfo = Reflection::Static::Reflect(object);

					data["Type"] = typeInfo.GetName();

					typeInfo.TraverseMemberVariableAndDo
					(
						[&](auto& field)
						{
							auto& member = field.Invoke(object);

							using type = typename std::remove_cv_t<std::remove_reference_t<decltype(field)>>::Type;

							if constexpr (std::is_fundamental_v<type>)
							{
								data[field.GetName()] = member;
							}
							else
							{
								data[field.GetName()] = SerializeHelper<type>::SerializeToJson(member);
							}
						}
					);

					return data;
				}
			};

			// 特化版本，用于处理string
			template <>
			struct SerializeHelper<string>
			{
				static json SerializeToJson(const string& object)
				{
					return object;
				}
			};

			// 特化版本，用于处理unordered_map
			template <typename K, typename V>
			struct SerializeHelper<unordered_map<K, V>>
			{
				static json SerializeToJson(const unordered_map<K, V>& object)
				{
					json data;
					
					if constexpr (std::is_same_v<K, std::string>)
					{
						for (const auto& [key, value] : object)
						{
							if constexpr (std::is_arithmetic_v<V>)
								data[key] = value;
							else
								data[key] = SerializeHelper<V>::SerializeToJson(value);
						}
					}
					else if constexpr (std::is_arithmetic_v<K>)
					{
						for (const auto& [key, value] : object)
						{
							if constexpr (std::is_arithmetic_v<V>)
								data[std::to_string(key)] = value;
							else
								data[std::to_string(key)] = SerializeHelper<V>::SerializeToJson(value);
						}
					}
					else
					{
						Debug::LogWarning("Serialize failed, unsupported key type in unordered_map.");
					}

					return data;
				}
			};
		}

		template <typename T>
		static string Serialize(const T& object)
		{
			json data = Internal::SerializeHelper<T>::SerializeToJson(object);
			return data.dump(4);
		}

		namespace Internal
		{
			template <typename T>
			struct DeserializeHelperV1
			{
				static T DeserializeFromJson(json data)
				{
					T object{};

					auto typeInfo = Reflection::Static::Reflect<T>();

					typeInfo.TraverseMemberVariableAndDo
					(
						[&](auto& field)
						{
							if (!data[field.GetName()].is_null())
							{
								auto& member = field.Invoke(object);

								using type = typename std::remove_cv_t<std::remove_reference_t<decltype(field)>>::Type;

								if constexpr (std::is_fundamental_v<type>)
								{
									member = data[field.GetName()];
								}
								else
								{
									member = DeserializeHelperV1<type>::DeserializeFromJson(data[field.GetName()]);
								}
							}
						}
					);

					return object;
				}
			};

			// 特化版本，用于处理string
			template <>
			struct DeserializeHelperV1<string>
			{
				static string DeserializeFromJson(json data)
				{
					return data.get<string>();
				}
			};

			// 特化版本，用于处理unordered_map
			template <typename K, typename V>
			struct DeserializeHelperV1<unordered_map<K, V>>
			{
				static unordered_map<K, V> DeserializeFromJson(json data)
				{
					unordered_map<K, V> object;

					if constexpr (std::is_same_v<K, std::string>)
					{
						for (auto& [key, value] : data.items())
						{
							if constexpr (std::is_arithmetic_v<V>)
								object[key] = value.get<V>();
							else
								object[key] = DeserializeHelperV1<V>::DeserializeFromJson(value);
						}
					}
					else if constexpr (std::is_integral_v<K>)
					{
						for (auto& [key, value] : data.items())
						{
							if constexpr (std::is_arithmetic_v<V>)
								object[static_cast<K>(std::stoi(key))] = value.get<V>();
							else
								object[static_cast<K>(std::stoi(key))] = DeserializeHelperV1<V>::DeserializeFromJson(value);
						}
					}
					else if constexpr (std::is_floating_point_v<K>)
					{
						for (auto& [key, value] : data.items())
						{
							if constexpr (std::is_arithmetic_v<V>)
								object[static_cast<K>(std::stof(key))] = value.get<V>();
							else
								object[static_cast<K>(std::stof(key))] = DeserializeHelperV1<V>::DeserializeFromJson(value);
						}
					}
					else
					{
						Debug::LogWarning("Deserialize failed, unsupported key type in unordered_map.");
					}
					
					return object;
				}
			};
		}

		template <typename T>
		static T DeserializeFromJson(json data)
		{
			return Internal::DeserializeHelperV1<T>::DeserializeFromJson(data);
		}

		template <typename T>
		static T Deserialize(const string& data)
		{
			json jsonData = json::parse(data);
			return DeserializeFromJson<T>(jsonData);
		}

		namespace Internal
		{
			template <typename T>
			struct DeserializeHelperV2
			{
				static void DeserializeFromJson(T& object, json data)
				{
					auto typeInfo = Reflection::Static::Reflect<T>();

					typeInfo.TraverseMemberVariableAndDo
					(
						[&](auto& field)
						{
							if (!data[field.GetName()].is_null())
							{
								auto& member = field.Invoke(object);

								using type = typename std::remove_cv_t<std::remove_reference_t<decltype(field)>>::Type;

								if constexpr (std::is_fundamental_v<type>)
								{
									member = data[field.GetName()];
								}
								else
								{
									DeserializeHelperV2<type>::DeserializeFromJson(member, data[field.GetName()]);
								}
							}
						}
					);
				}
			};

			// 特化版本，用于处理string
			template <>
			struct DeserializeHelperV2<string>
			{
				static void DeserializeFromJson(string& object, json data)
				{
					object = data.get<string>();
				}
			};

			// 特化版本，用于处理unordered_map
			template <typename K, typename V>
			struct DeserializeHelperV2<unordered_map<K, V>>
			{
				static void DeserializeFromJson(unordered_map<K, V>& object, json data)
				{
					if constexpr (std::is_same_v<K, std::string>)
					{
						for (auto& [key, value] : data.items())
						{
							if constexpr (std::is_arithmetic_v<V>)
								object[key] = value.get<V>();
							else
								DeserializeHelperV2<V>::DeserializeFromJson(object[key], value);
						}
					}
					else if constexpr (std::is_integral_v<K>)
					{
						for (auto& [key, value] : data.items())
						{
							if constexpr (std::is_arithmetic_v<V>)
								object[static_cast<K>(std::stoi(key))] = value.get<V>();
							else
								DeserializeHelperV2<V>::DeserializeFromJson(object[static_cast<K>(std::stoi(key))], value);
						}
					}
					else if constexpr (std::is_floating_point_v<K>)
					{
						for (auto& [key, value] : data.items())
						{
							if constexpr (std::is_arithmetic_v<V>)
								object[static_cast<K>(std::stof(key))] = value.get<V>();
							else
								DeserializeHelperV2<V>::DeserializeFromJson(object[static_cast<K>(std::stof(key))], value);
						}
					}
					else
					{
						Debug::LogWarning("Deserialize failed, unsupported key type in unordered_map.");
					}
				}
			};
		}

		template <typename T>
		static void DeserializeFromJson(T& object, json data)
		{
			Internal::DeserializeHelperV2<T>::DeserializeFromJson(object, data);
		}

		template <typename T>
		static void Deserialize(T& object, const string& data)
		{
			json jsonData = json::parse(data);
			DeserializeFromJson(object, jsonData);
		}
	}
}